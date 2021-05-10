// Copyright 2021 Schimpe
#include "LaserScan.h"

namespace TodStandardEntities {

Entity LaserScan::create(std::shared_ptr<Scene> scene, const std::string &name,
                         Entity parent, const std::string& packagePath) {
    Entity laserScanEntity = scene->CreateEntity(name);
    unsigned int shaderProgram = ShaderSystem::createShaderProgram(
        (packagePath + "/resources/OpenGL/shaders/shader.vert").c_str(),
        (packagePath + "/resources/OpenGL/shaders/shader.frag").c_str());
    Mesh mesh = Mesh::nonEmptyMesh();
    auto& renderable = laserScanEntity.AddComponent<RenderableElementComponent>(shaderProgram, mesh, GL_POINTS);
    renderable.PointSize = 7.0f;
    laserScanEntity.AddComponent<ExpirableComponent>(1000);
    laserScanEntity.AddComponent<DynamicDataComponent>();
    laserScanEntity.GetComponent<TransformComponent>().setParent(parent);
    return laserScanEntity;
}

void LaserScan::onLaserScanReceived(const sensor_msgs::LaserScanConstPtr& msg, Entity &entity) {
    auto &dynamic = entity.GetComponent<DynamicDataComponent>();
    std::lock_guard<std::mutex> lock(*dynamic.Mutex);
    auto &expirable = entity.GetComponent<ExpirableComponent>();
    expirable.restamp();
    auto &renderable = entity.GetComponent<RenderableElementComponent>();
    auto &mesh = renderable.Meshes.front();
    mesh.Vertices.clear();
    for (int i=0; i < msg->ranges.size(); ++i) {
        float range = msg->ranges.at(i);
        if (!isnan(range)) {
            float angle = msg->angle_min + i * msg->angle_increment;
            float coordX = range * std::cos(angle);
            float coordY = range * std::sin(angle);
            mesh.Vertices.emplace_back(glm::vec3(coordX, coordY, 0.0f),
                                       glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }
    dynamic.HasNewData = true;
}

}; // namespace TodStandardEntities
