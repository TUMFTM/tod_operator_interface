// Copyright 2021 TUMFTM
#include "Interface.h"

Interface::Interface(int argc, char **argv) : Application(argc, argv) {
    CreateScene();
}

void Interface::CreateScene() {
    _activeScene = std::make_shared<Scene>();
    Entity CosysOrigin, CosysBaseFootprint;

    std::map<std::string, Entity> mapOfRelativeCosysEntities;
    CreateCosysEntities(CosysOrigin, CosysBaseFootprint, mapOfRelativeCosysEntities);
    CreateDisplayEntities(CosysBaseFootprint);
    CreateGridAndFloorEntities(CosysBaseFootprint);
    CreateCameraFramebufferAndTopViewEntities(CosysBaseFootprint);
    CreateVideoEntities(CosysBaseFootprint);
    CreateLaneEntities(CosysBaseFootprint);
    CreateLaserScanEntities(mapOfRelativeCosysEntities);

    std::string vehicleID{""};
    _ros->getParam(_ros->getNodeName() + "/vehicleID", vehicleID);
    bool couldDeserialize = DeserializeEntityData(vehicleID);
    CreateVideoMeshes(couldDeserialize, vehicleID);
}

void Interface::CreateCosysEntities(Entity &CosysOrigin, Entity &CosysBaseFootprint,
                                    std::map<std::string, Entity> &mapOfRelativeCosysEntities) {
    CosysOrigin = TodStandardEntities::CoordinateSystem::create(
        _activeScene, "CosysOrigin", RosInterface::getPackagePath());

    CosysBaseFootprint = TodStandardEntities::CoordinateSystem::create(
        _activeScene, "base_footprint", RosInterface::getPackagePath());
    CosysBaseFootprint.AddComponent<DynamicDataComponent>();
    CosysBaseFootprint.GetComponent<DynamicDataComponent>().HasNewData = true; // for first render loop iteration
    _activeScene->setBaseFootPrint(CosysBaseFootprint.GetHandle());
    _ros->addSubscriber<nav_msgs::Odometry>("/Operator/VehicleBridge/odometry",
                                            TodStandardEntities::BaseFootprint::onPositionUpdate, CosysBaseFootprint);

    mapOfRelativeCosysEntities =
        TodStandardEntities::CoordinateSystem::createTransformTree(
            _activeScene, RosInterface::getPackagePath(), CosysBaseFootprint);
}



void Interface::CreateDisplayEntities(const Entity &CosysBaseFootprint) {
    Entity velocityDisplay = TodStandardEntities::Display::create(
        _activeScene, "Velocity Display", CosysBaseFootprint, RosInterface::getPackagePath());
    float displayHeight{ 0.20f };
    float displayX{ 1.75f };
    velocityDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, -0.45f, displayHeight));
    _ros->addSubscriber<tod_msgs::VehicleData>("/Operator/VehicleBridge/vehicle_data",
                                               TodStandardEntities::Display::onSpeedUpdate, velocityDisplay);

    Entity desiredVelocityDisplay = TodStandardEntities::Display::create(
        _activeScene, "Desired Velocity Display", CosysBaseFootprint, RosInterface::getPackagePath());
    desiredVelocityDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.0f, displayHeight));
    _ros->addSubscriber<tod_msgs::PrimaryControlCmd>(
        "primary_control_cmd", TodStandardEntities::Display::onDesiredSpeedUpdate, desiredVelocityDisplay);

    Entity gearDisplay = TodStandardEntities::Display::create(
        _activeScene, "Gear Display", CosysBaseFootprint, RosInterface::getPackagePath());
    gearDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.3f, displayHeight));
    _ros->addSubscriber<tod_msgs::VehicleData>("/Operator/VehicleBridge/vehicle_data",
                                               TodStandardEntities::Display::onGearUpdate, gearDisplay);

    Entity desiredGearDisplay = TodStandardEntities::Display::create(
        _activeScene, "Desired Gear Display", CosysBaseFootprint, RosInterface::getPackagePath());
    desiredGearDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.5f, displayHeight));
    _ros->addSubscriber<tod_msgs::SecondaryControlCmd>(
        "secondary_control_cmd", TodStandardEntities::Display::onDesiredGearUpdate, desiredGearDisplay);
}

void Interface::CreateGridAndFloorEntities(const Entity &CosysBaseFootprint) {
    Entity grid = TodStandardEntities::Grid::create(
        _activeScene, "Grid", RosInterface::getPackagePath());
    Entity floor = TodStandardEntities::Floor::create(
        _activeScene, "Floor", RosInterface::getPackagePath(), CosysBaseFootprint);
}

void Interface::CreateCameraFramebufferAndTopViewEntities(const Entity &CosysBaseFootprint) {
    Entity camera = TodStandardEntities::Camera::create(_activeScene, "VehicleFollowCamera", CosysBaseFootprint);
    Entity mainFramebuffer = _activeScene->CreateEntity("MainFramebuffer");
    auto& framebuffer = mainFramebuffer.AddComponent<FrameBufferComponent>(true);
    framebuffer.CameraEntity = camera.GetHandle();
    Entity topView = TodStandardEntities::TopView::create(_activeScene, "TopView", CosysBaseFootprint);
}



void Interface::CreateVideoEntities(const Entity &CosysBaseFootprint) {
    std::vector<Entity> videos;
    for (int i=0; i <= 10; ++i) {
        std::string paramName = std::string(RosInterface::getNodeName() + "/camera" + std::to_string(i) + "/name");
        if (_ros->hasParam(paramName)) {
            std::string videoName{""};
            _ros->getParam(paramName, videoName);
            videoName.erase(videoName.begin()); // removes '/'
            bool isFisheye;
            _ros->getParam(std::string(RosInterface::getNodeName() + "/camera" + std::to_string(i) + "/is_fisheye"),
                           isFisheye);
            videos.emplace_back(TodStandardEntities::Video::create(
                _activeScene, videoName, isFisheye, CosysBaseFootprint));
            std::string imageTopic = "/Operator/Video/" + videoName + "/image_raw";
            _ros->addSubscriber<sensor_msgs::Image>(
                imageTopic, TodStandardEntities::Video::onImageReceived, videos.back());
            std::string projectionTopic = "/Operator/Projection/" + videoName + "/image_raw";
            _ros->addSubscriber<sensor_msgs::Image>(
                projectionTopic, TodStandardEntities::Video::onProjectionReceived, videos.back());
        }
    }
}

void Interface::CreateLaneEntities(const Entity &CosysBaseFootprint) {
    Entity leftLane = TodStandardEntities::Path::create(_activeScene, "Left Front Lane",
                                                        CosysBaseFootprint, RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>("/Operator/Projection/vehicle_lane_front_left",
                                        TodStandardEntities::Path::onPathReceived, leftLane);
    Entity rightLane = TodStandardEntities::Path::create(_activeScene, "Right Front Lane",
                                                         CosysBaseFootprint, RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>("/Operator/Projection/vehicle_lane_front_right",
                                        TodStandardEntities::Path::onPathReceived, rightLane);
}

void Interface::CreateLaserScanEntities(const std::map<std::string, Entity> &mapOfRelativeCosysEntities) {
    if (mapOfRelativeCosysEntities.find("CosysLidarFront") != mapOfRelativeCosysEntities.end()) {
        Entity frontLaserScan = TodStandardEntities::LaserScan::create(
            _activeScene, "Lidar Front Scan", mapOfRelativeCosysEntities.at("CosysLidarFront"),
            RosInterface::getPackagePath());
        _ros->addSubscriber<sensor_msgs::LaserScan>(
            "/Operator/Lidar/LidarFront/scan",
            TodStandardEntities::LaserScan::onLaserScanReceived, frontLaserScan);
    }
    if (mapOfRelativeCosysEntities.find("CosysLidarRear") != mapOfRelativeCosysEntities.end()) {
        Entity rearLaserScan = TodStandardEntities::LaserScan::create(
            _activeScene, "Lidar Rear Scan", mapOfRelativeCosysEntities.at("CosysLidarRear"),
            RosInterface::getPackagePath());
        _ros->addSubscriber<sensor_msgs::LaserScan>(
            "/Operator/Lidar/LidarRear/scan",
            TodStandardEntities::LaserScan::onLaserScanReceived, rearLaserScan);
    }
}


bool Interface::DeserializeEntityData(const std::string &vehicleID) {
    auto view = _activeScene->_registry.view<TagComponent, VideoComponent>();
    std::vector<Entity> entities;
    for (auto& entityHandle : view)
        entities.emplace_back(Entity(entityHandle, _activeScene.get()));
    bool ret = Serialization::DeserializeEntities(_ros->getPackagePath(), vehicleID, entities);
    if (!ret)
        ROS_WARN("%s: First launch of hmi with vehicle id %s.", _ros->getNodeName().c_str(), vehicleID.c_str());
    return ret;
}

void Interface::CreateVideoMeshes(const bool couldDeserialize, const std::string &vehicleID) {
    auto videoHandles = _activeScene->_registry.view<VideoComponent>();
    std::vector<Entity> videoEntities;
    for (auto videoHandle : videoHandles)
        videoEntities.emplace_back(Entity(videoHandle, _activeScene.get()));
    std::vector<std::future<void>> futures;
    for (auto& videoEntity : videoEntities) {
        auto &video = videoEntity.GetComponent<VideoComponent>();
        auto &transform = videoEntity.GetComponent<TransformComponent>();
        geometry_msgs::TransformStamped tf2cam = _ros->tfLookup(video.CameraName, "base_footprint");
        if (!couldDeserialize) {
            // if data could not be deserialized, i.e., first launch with this vehicle,
            // move video streams next to each other
            if (!video.IsFisheye) {
                static int shiftNonFisheye{0};
                transform.setTranslation(glm::vec3(5.0f, 1.5f - float(shiftNonFisheye++), 0.0f));
            } else {
                static int shiftFisheye{0};
                transform.setTranslation(glm::vec3(5.0f, 1.5f - float(shiftFisheye++), 1.0f));
            }
        }
        // initialize mesh depending on camera type
        if (!video.IsFisheye) {
            PinholeModel camMdl(video.CameraName, vehicleID);
            _ros->getParam(_ros->getNodeName() + "/" + video.CameraName + "/image_width", video.WidthRaw);
            _ros->getParam(_ros->getNodeName() + "/" + video.CameraName + "/image_height", video.HeightRaw);
            futures.emplace_back(
                std::async(std::launch::async,
                           TodStandardEntities::Video::init_mesh_from_pinhole_model,
                           std::ref(videoEntity), camMdl, tf2cam));
        } else {
            OcamModel camMdl(video.CameraName, vehicleID);
            video.WidthRaw = camMdl.width_raw;
            video.HeightRaw = camMdl.height_raw;
            futures.emplace_back(
                std::async(std::launch::async,
                           TodStandardEntities::Video::init_mesh_from_ocam_model,
                           std::ref(videoEntity), camMdl, tf2cam));
        }
    }
}
