// Copyright 2021 Feiler
// PERCEPTION MODIFICATION
#pragma once

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Systems/ShaderSystem.h"
#include "tod_msgs/Mesh.h"
#include <string>
#include <memory>
#include <vector>

namespace TodStandardEntities {

class MeshEntity {
public:
    static Entity create(std::shared_ptr<Scene> scene, const std::string &name,
                         Entity parent, const std::string& packagePath);
    static void onRouteReceived(const tod_msgs::MeshConstPtr& msg, Entity &entity);
    static void processMeshMsg(
        const tod_msgs::MeshConstPtr& msg,
        Entity& entity);
    static void clearVerticesAndIndicesOfAllMeshes(
        RenderableElementComponent& renderables);
    static void addMeshMsgIntoMesh(Mesh& mesh,
                                   const tod_msgs::MeshConstPtr& meshMsg,
                                   const std::string& entityTagName);

private:
    MeshEntity() = default;
    static void addVerticesToMesh(Mesh& mesh,
                                  const std::vector<geometry_msgs::Point>& vertices);
    static bool colorsShouldBeAdded(const unsigned int& verticesSize,
                                    const unsigned int& colorsSize,
                                    const std::string& entityTagName);
    static void fillColorsInMesh(Mesh& mesh,
                                 const std::vector<tod_msgs::Color>& colors);
    static void addIndicesToMesh(Mesh& mesh,
                                 const std::vector<unsigned int>& indices);
};

}; // namespace TodStandardEntities
