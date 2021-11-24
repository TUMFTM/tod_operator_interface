// Copyright 2021 Feiler
// PERCEPTION MODIFICATION
#include "MeshEntity.h"

namespace TodStandardEntities {

Entity MeshEntity::create(std::shared_ptr<Scene> scene, const std::string &name,
                         Entity parent, const std::string& packagePath) {
    Entity routeEntity = scene->CreateEntity(name);
    auto& transform = routeEntity.GetComponent<TransformComponent>();
    unsigned int shaderProgram = ShaderSystem::createShaderProgram(
        (packagePath + "/resources/OpenGL/shaders/shader.vert").c_str(),
        (packagePath + "/resources/OpenGL/shaders/shaderTransparent.frag").c_str());
    Mesh mesh = Mesh::nonEmptyMesh();
    auto& renderable = routeEntity.AddComponent<RenderableElementComponent>(
        shaderProgram, mesh);
    routeEntity.AddComponent<ExpirableComponent>(1000);
    transform.setParent(parent);
    renderable.LineWidth = 5.0f;
    renderable.PointSize = 7.0f;
    transform.setTranslation(glm::vec3(0.0f, 0.0f, 0.022f)); // move up
    routeEntity.AddComponent<DynamicDataComponent>();
    return routeEntity;
}

void MeshEntity::onRouteReceived(const tod_msgs::MeshConstPtr& msg, Entity &entity) {
    auto& renderables = entity.GetComponent<RenderableElementComponent>();
    auto& expirable = entity.GetComponent<ExpirableComponent>();
    auto& dynamic = entity.GetComponent<DynamicDataComponent>();
    std::lock_guard<std::mutex> lock(*dynamic.Mutex);
    expirable.restamp();
    dynamic.HasNewData = true;
    clearVerticesAndIndicesOfAllMeshes(renderables);
    auto &mesh = renderables.Meshes.front();
    addMeshMsgIntoMesh(mesh, msg, entity.GetComponent<TagComponent>().Tag);
}

void MeshEntity::processMeshMsg(
    const tod_msgs::MeshConstPtr& msg,
    Entity& entity) {
    auto& renderables = entity.GetComponent<RenderableElementComponent>();
    auto& expirable = entity.GetComponent<ExpirableComponent>();
    auto& dynamic = entity.GetComponent<DynamicDataComponent>();
    std::lock_guard<std::mutex> lock(*dynamic.Mutex);

    dynamic.HasNewData = true;
    expirable.restamp();

    clearVerticesAndIndicesOfAllMeshes(renderables);
    auto &mesh = renderables.Meshes.front();
    addMeshMsgIntoMesh(mesh, msg, entity.GetComponent<TagComponent>().Tag);
}

void MeshEntity::clearVerticesAndIndicesOfAllMeshes(
    RenderableElementComponent& renderables) {
    std::for_each(renderables.Meshes.begin(), renderables.Meshes.end(),
                  [](Mesh& mesh){
                      mesh.Vertices.clear();
                      mesh.Indices.clear();
                  });
}

void MeshEntity::addMeshMsgIntoMesh(Mesh& mesh,
                                          const tod_msgs::MeshConstPtr& meshMsg,
                                          const std::string& entityTagName) {
    addVerticesToMesh(mesh, meshMsg->vertices);
    if ( colorsShouldBeAdded(meshMsg->vertices.size(), meshMsg->colors.size(),
                            entityTagName) ) {
        fillColorsInMesh(mesh, meshMsg->colors);
    }
    addIndicesToMesh(mesh, meshMsg->indices);
}

void MeshEntity::addVerticesToMesh(Mesh& mesh,
                                         const std::vector<geometry_msgs::Point>& vertices) {
    std::for_each(std::begin(vertices), std::end(vertices),
                  [&mesh](const geometry_msgs::Point& vertex){
                      mesh.Vertices.push_back(Vertex(glm::vec3(
                          (float) vertex.x,
                          (float) vertex.y,
                          (float) vertex.z)));
                  });
}

bool MeshEntity::colorsShouldBeAdded(const unsigned int& verticesSize,
                                           const unsigned int& colorsSize,
                                           const std::string& entityTagName) {
    bool addColorsToMesh{ true };
    if ( verticesSize != colorsSize ) {
        ROS_ERROR("in callback from entity %s: colors.size() != vertices.size()."
                  " Colors are discarded", entityTagName.c_str());
        addColorsToMesh = false;
    }
    return addColorsToMesh;
}

void MeshEntity::fillColorsInMesh(Mesh& mesh,
                                        const std::vector<tod_msgs::Color>& colors) {
    for ( int iterator = 0; iterator != colors.size(); ++iterator ) {
        mesh.Vertices.at(iterator).TexColor.x = colors.at(iterator).r;
        mesh.Vertices.at(iterator).TexColor.y = colors.at(iterator).g;
        mesh.Vertices.at(iterator).TexColor.z = colors.at(iterator).b;
    }
}

void MeshEntity::addIndicesToMesh(Mesh& mesh,
                                        const std::vector<unsigned int>& indices) {
    std::for_each(std::begin(indices), std::end(indices),
                  [&mesh](const unsigned int& index){
                      mesh.Indices.push_back(index);
                  });
}

}; // namespace TodStandardEntities
