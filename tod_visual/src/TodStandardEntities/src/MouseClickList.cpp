// Copyright 2021 Feiler
// PERCEPTION MODIFICATION
#include "MouseClickList.h"
#include "MeshEntity.h"

namespace TodStandardEntities {

Entity MouseClickList::create(std::shared_ptr<Scene> scene,
                              std::string name, Entity parent, const std::string& packagePath) {
    Entity mouseClickList = scene->CreateEntity(name);
    unsigned int shaderProgram = ShaderSystem::createShaderProgram(
        (packagePath + "/resources/OpenGL/shaders/shader.vert").c_str(),
        (packagePath + "/resources/OpenGL/shaders/shader.frag").c_str());
    Mesh mesh = Mesh::nonEmptyMesh();
    auto& renderable = mouseClickList.AddComponent<RenderableElementComponent>(
        shaderProgram, mesh, GL_POINTS);
    renderable.PointSize = 5.0f;
    mouseClickList.GetComponent<TransformComponent>().setParent(parent);
    mouseClickList.AddComponent<DynamicDataComponent>();
    return mouseClickList;
}

void MouseClickList::onMouseClickListReceived(const sensor_msgs::PointCloudConstPtr& points,
                                              Entity &entity) {
    auto& renderables = entity.GetComponent<RenderableElementComponent>();
    auto& dynamic = entity.GetComponent<DynamicDataComponent>();
    std::lock_guard<std::mutex> lock(*dynamic.Mutex);
    dynamic.HasNewData = true;

    TodStandardEntities::MeshEntity::clearVerticesAndIndicesOfAllMeshes(renderables);
    for ( auto& point : points->points ) {
        renderables.Meshes.front().Vertices.push_back(Vertex(glm::vec3(
            point.x, point.y, point.z)));
        renderables.Meshes.front().Vertices.back().TexColor =
            glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

}   // namespace TodStandardEntities
