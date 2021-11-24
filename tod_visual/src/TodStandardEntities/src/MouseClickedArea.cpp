// Copyright 2021 Feiler
// PERCEPTION MODIFICATION
#include "MouseClickedArea.h"
#include "MeshEntity.h"

namespace TodStandardEntities {

Entity MouseClickedArea::create(std::shared_ptr<Scene> scene,
            std::string name, Entity parent, const std::string& packagePath) {
    Entity mouseClickArea = scene->CreateEntity(name);
    unsigned int shaderProgram = ShaderSystem::createShaderProgram(
                (packagePath + "/resources/OpenGL/shaders/shader.vert").c_str(),
                (packagePath + "/resources/OpenGL/shaders/shaderTransparent.frag").c_str());
    std::vector<unsigned int> indices { 0, 1, 2 };
    std::vector<Vertex> vertices = Mesh::VectorOfThreeZeroVertices();
    Mesh mesh(vertices, indices);
    auto& renderable = mouseClickArea.AddComponent<RenderableElementComponent>(
            shaderProgram, mesh);
    auto& transform = mouseClickArea.GetComponent<TransformComponent>();
    transform.setParent(parent);
    mouseClickArea.AddComponent<DynamicDataComponent>();
    return mouseClickArea;
}

void MouseClickedArea::onMouseClickListReceived(const sensor_msgs::PointCloudConstPtr& points,
        Entity &entity) {
    auto& renderables = entity.GetComponent<RenderableElementComponent>();
    auto& dynamic = entity.GetComponent<DynamicDataComponent>();
    std::lock_guard<std::mutex> lock(*dynamic.Mutex);
    dynamic.HasNewData = true;

    TodStandardEntities::MeshEntity::clearVerticesAndIndicesOfAllMeshes(renderables);
    int lastIndex { 0 };
    for ( int currentIndex = 0; currentIndex != points->points.size();
                    ++currentIndex) {
        auto& point = points->points.at(currentIndex);
        renderables.Meshes.front().Vertices.push_back(Vertex(glm::vec3(
            point.x, point.y, point.z)));
        renderables.Meshes.front().Indices.push_back(0);
        renderables.Meshes.front().Indices.push_back(lastIndex);
        renderables.Meshes.front().Indices.push_back(currentIndex);
        lastIndex = currentIndex;
        renderables.Meshes.front().Vertices.back().TexColor =
            glm::vec3(1.0f, 165.0f/255.0f, 0.0f);
    }
}

}   // namespace TodStandardEntities
