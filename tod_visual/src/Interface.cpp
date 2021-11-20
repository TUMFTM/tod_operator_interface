// Copyright 2021 TUMFTM
#include "Interface.h"
Interface::Interface(int argc, char **argv) : Application(argc, argv) {
    CreateScene();
}

void Interface::CreateScene() {
    _activeScene = std::make_shared<Scene>();

    std::map<std::string, Entity> coordinateSystems;
    CreateCosysEntities(coordinateSystems);
    CreateCameraFramebufferAndTopViewEntities(coordinateSystems);
    CreateDisplayEntities(coordinateSystems);
    CreateVehicleModelEntities(coordinateSystems);
    CreateGridAndFloorEntities(coordinateSystems);
    CreateVideoEntities(coordinateSystems);
    CreateLaneEntities(coordinateSystems);
    CreateLaserScanEntities(coordinateSystems);

    std::string vehicleID{""};
    _ros->getParam(_ros->getNodeName() + "/vehicleID", vehicleID);
    bool couldDeserialize = DeserializeEntityData(vehicleID);
    CreateVideoMeshes(couldDeserialize, vehicleID);
}

void Interface::CreateCosysEntities(std::map<std::string, Entity> &coordinateSystems) {
    coordinateSystems.insert_or_assign(
        "ftm", TodStandardEntities::CoordinateSystem::create(
                   _activeScene, "ftm", RosInterface::getPackagePath()));

    coordinateSystems.insert_or_assign("base_footprint",
                                       TodStandardEntities::CoordinateSystem::create(
                                           _activeScene, "base_footprint",
                                           RosInterface::getPackagePath())).first->second;
    coordinateSystems.at("base_footprint").AddComponent<DynamicDataComponent>();
    // has new data for first render loop iteration
    coordinateSystems.at("base_footprint").GetComponent<DynamicDataComponent>().HasNewData = true;
    _activeScene->setBaseFootPrint(coordinateSystems.at("base_footprint").GetHandle());
    _ros->addSubscriber<nav_msgs::Odometry>("/Operator/VehicleBridge/odometry",
                                            TodStandardEntities::BaseFootprint::onPositionUpdate,
                                            coordinateSystems.at("base_footprint"));

    TodStandardEntities::CoordinateSystem::createTransformTree(
        coordinateSystems, _activeScene,
        RosInterface::getPackagePath(), coordinateSystems.at("base_footprint"));
}

void Interface::CreateDisplayEntities(const std::map<std::string, Entity> &coordinateSystems) {
    Entity velocityDisplay = TodStandardEntities::Display::create(
        _activeScene, "Velocity Display", coordinateSystems.at("base_footprint"), RosInterface::getPackagePath());
    float displayHeight{ 0.20f };
    float displayX{ 1.75f };
    velocityDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, -0.45f, displayHeight));
    _ros->addSubscriber<tod_msgs::VehicleData>("/Operator/VehicleBridge/vehicle_data",
                                               TodStandardEntities::Display::onSpeedUpdate, velocityDisplay);

    Entity desiredVelocityDisplay = TodStandardEntities::Display::create(
        _activeScene, "Desired Velocity Display", coordinateSystems.at("base_footprint"),
        RosInterface::getPackagePath());
    desiredVelocityDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.0f, displayHeight));
    _ros->addSubscriber<tod_msgs::PrimaryControlCmd>(
        "primary_control_cmd", TodStandardEntities::Display::onDesiredSpeedUpdate, desiredVelocityDisplay);

    Entity gearDisplay = TodStandardEntities::Display::create(
        _activeScene, "Gear Display", coordinateSystems.at("base_footprint"), RosInterface::getPackagePath());
    gearDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.3f, displayHeight));
    _ros->addSubscriber<tod_msgs::VehicleData>("/Operator/VehicleBridge/vehicle_data",
                                               TodStandardEntities::Display::onGearUpdate, gearDisplay);

    Entity desiredGearDisplay = TodStandardEntities::Display::create(
        _activeScene, "Desired Gear Display", coordinateSystems.at("base_footprint"), RosInterface::getPackagePath());
    desiredGearDisplay.GetComponent<TransformComponent>().setTranslation(glm::vec3(displayX, 0.5f, displayHeight));
    _ros->addSubscriber<tod_msgs::SecondaryControlCmd>(
        "secondary_control_cmd", TodStandardEntities::Display::onDesiredGearUpdate, desiredGearDisplay);
}

void Interface::CreateVehicleModelEntities(const std::map<std::string, Entity> &coordinateSystems) {
    std::string vehicleID;
    float track_width{0};
    float distance_front_axle{0};
    float distance_rear_axle{0};
    _ros->getParam(_ros->getNodeName() + "/vehicleID", vehicleID);
    _ros->getParam(_ros->getNodeName() + "/track_width", track_width);
    _ros->getParam(_ros->getNodeName() + "/distance_front_axle", distance_front_axle);
    _ros->getParam(_ros->getNodeName() + "/distance_rear_axle", distance_rear_axle);

    std::string modelPath = "/vehicle_config/" + vehicleID + "/model-mesh/";
    tofGL::ModelLoader *loader = new tofGL::ModelLoader(_activeScene,
                                                        RosInterface::getPackagePath("tod_vehicle_config")
                                                            + modelPath);
    Entity vehicleModel = loader->loadModel("model_chassis", coordinateSystems.at("base_footprint"),
                                            glm::vec3(1.0f, 1.0f, 0.01f),
                                            glm::vec3(glm::radians(0.0f), 0.0f, 0.0f),
                                            RosInterface::getPackagePath());
    vehicleModel.GetComponent<TransformComponent>().setTranslation(glm::vec3(0.0f, 0.0f, 0.025f));
    Entity steeringWheel = loader->loadModel("model_steeringWheel", coordinateSystems.at("base_footprint"),
                                             glm::vec3(1.0f, 1.0f, 1.0f),
                                             glm::vec3(0.0f, 0.0f , 0.0f),
                                             RosInterface::getPackagePath());
    Entity wheelFrontLeft = loader->loadModel("model_wheel", coordinateSystems.at("base_footprint"),
                                              glm::vec3(1.0f, 1.0f, 0.1f),
                                              glm::vec3(0.0f, 0.0f, glm::radians(0.0f)),
                                              RosInterface::getPackagePath());
    Entity wheelFrontRight = loader->loadModel("model_wheel", coordinateSystems.at("base_footprint"),
                                               glm::vec3(1.0f, 1.0f, 0.1f),
                                               glm::vec3(0.0f, 0.0f, glm::radians(180.0f)),
                                               RosInterface::getPackagePath());
    Entity wheelRearLeft = loader->loadModel("model_wheel", coordinateSystems.at("base_footprint"),
                                             glm::vec3(1.0f, 1.0f, 0.1f),
                                             glm::vec3(0.0f, 0.0f, glm::radians(0.0f)),
                                             RosInterface::getPackagePath());
    Entity wheelRearRight = loader->loadModel("model_wheel", coordinateSystems.at("base_footprint"),
                                              glm::vec3(1.0f, 1.0f, 0.1f),
                                              glm::vec3(0.0f, 0.0f, glm::radians(180.0f)),
                                              RosInterface::getPackagePath());
    wheelFrontLeft.GetComponent<TransformComponent>().setTranslation(
        glm::vec3(distance_front_axle, track_width/2.0, 0.0f));
    steeringWheel.GetComponent<TransformComponent>().setTranslation(glm::vec3(0.3f, 0.3f, 0.3f));
    wheelFrontRight.GetComponent<TransformComponent>().setTranslation(
        glm::vec3(distance_front_axle, -track_width/2.0, 0.0f));
    wheelRearLeft.GetComponent<TransformComponent>().setTranslation(
        glm::vec3(-distance_rear_axle, track_width/2.0, 0.0f));
    wheelRearRight.GetComponent<TransformComponent>().setTranslation(
        glm::vec3(-distance_rear_axle, -track_width/2.0, 0.0f));
}

void Interface::CreateGridAndFloorEntities(const std::map<std::string, Entity> &coordinateSystems) {
    Entity grid = TodStandardEntities::Grid::create(
        _activeScene, "Grid", RosInterface::getPackagePath());
    Entity floor = TodStandardEntities::Floor::create(
        _activeScene, "Floor", RosInterface::getPackagePath(), coordinateSystems.at("base_footprint"));
}

void Interface::CreateCameraFramebufferAndTopViewEntities(const std::map<std::string, Entity> &coordinateSystems) {
    Entity camera = TodStandardEntities::Camera::create(
        _activeScene, "VehicleFollowCamera", coordinateSystems.at("base_footprint"));
    Entity mainFramebuffer = _activeScene->CreateEntity("MainFramebuffer");
    auto& framebuffer = mainFramebuffer.AddComponent<FrameBufferComponent>(true);
    framebuffer.CameraEntity = camera.GetHandle();
// callback switches camera position to lock backwards (gear rear, InvertSteeringInGearReverse in tod_command_creation.launch)
//    _ros->addSubscriber<tod_msgs::VehicleData>("/Operator/VehicleBridge/vehicle_data",
//                                               TodStandardEntities::Camera::onGearUpdate, camera);
}

void Interface::CreateVideoEntities(const std::map<std::string, Entity> &coordinateSystems) {
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
                _activeScene, videoName, isFisheye, coordinateSystems.at("base_footprint")));
            std::string imageTopic = "/Operator/Video/" + videoName + "/image_raw";
            _ros->addSubscriber<sensor_msgs::Image>(
                imageTopic, TodStandardEntities::Video::onImageReceived, videos.back());
            std::string projectionTopic = "/Operator/Projection/" + videoName + "/image_raw";
            _ros->addSubscriber<sensor_msgs::Image>(
                projectionTopic, TodStandardEntities::Video::onProjectionReceived, videos.back());
        }
    }
}

void Interface::CreateLaneEntities(const std::map<std::string, Entity> &coordinateSystems) {
    Entity leftLane = TodStandardEntities::Path::create(_activeScene, "Left Front Lane",
                                                        RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>(
        "/Operator/Projection/vehicle_lane_front_left",
        TodStandardEntities::Path::onPathReceived, leftLane, coordinateSystems);
    Entity rightLane = TodStandardEntities::Path::create(_activeScene, "Right Front Lane",
                                                         RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>(
        "/Operator/Projection/vehicle_lane_front_right",
        TodStandardEntities::Path::onPathReceived, rightLane, coordinateSystems);
    Entity rleftLane = TodStandardEntities::Path::create(_activeScene, "Left Rear Lane",
                                                         RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>(
        "/Operator/Projection/vehicle_lane_rear_left",
        TodStandardEntities::Path::onPathReceived, rleftLane, coordinateSystems);
    Entity rrightLane = TodStandardEntities::Path::create(_activeScene, "Right Rear Lane",
                                                          RosInterface::getPackagePath());
    _ros->addSubscriber<nav_msgs::Path>(
        "/Operator/Projection/vehicle_lane_rear_right",
        TodStandardEntities::Path::onPathReceived, rrightLane, coordinateSystems);
}

void Interface::CreateLaserScanEntities(const std::map<std::string, Entity> &coordinateSystems) {
    // TODO(Andi): parse sensors-lidar and initialize laser scan entities accordingly
    if (coordinateSystems.find("LidarFront") != coordinateSystems.end()) {
        Entity frontLaserScan = TodStandardEntities::LaserScan::create(
            _activeScene, "Lidar Front Scan", RosInterface::getPackagePath());
        _ros->addSubscriber<sensor_msgs::LaserScan>(
            "/Operator/Lidar/LidarFront/scan",
            TodStandardEntities::LaserScan::onLaserScanReceived, frontLaserScan, coordinateSystems);
    }
    if (coordinateSystems.find("LidarRear") != coordinateSystems.end()) {
        Entity rearLaserScan = TodStandardEntities::LaserScan::create(
            _activeScene, "Lidar Rear Scan", RosInterface::getPackagePath());
        _ros->addSubscriber<sensor_msgs::LaserScan>(
            "/Operator/Lidar/LidarRear/scan",
            TodStandardEntities::LaserScan::onLaserScanReceived, rearLaserScan, coordinateSystems);
    }
}


bool Interface::DeserializeEntityData(const std::string &vehicleID) {
    auto view = _activeScene->_registry.view<TagComponent, VideoComponent>();
    std::vector<Entity> entities;
    for (auto& entityHandle : view)
        entities.emplace_back(entityHandle, _activeScene.get());
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
