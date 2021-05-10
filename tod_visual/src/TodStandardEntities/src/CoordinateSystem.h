// Copyright 2020 TUMFTM
#pragma once
#include <ros/ros.h>
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Systems/ShaderSystem.h"
#include "tod_transform/CommonTransformTreePublisher.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace TodStandardEntities {

class CoordinateSystem {
public:
    static Entity create(std::shared_ptr<Scene> scene,
                         std::string name, const std::string& packagePath);
    static std::map<std::string, Entity> createTransformTree(
        std::shared_ptr<Scene> scene, const std::string &packagePath, Entity& cosysBaseFootPrint);

private:
    CoordinateSystem() = default;
    static void checkIfTagMatchesKey(Entity& tagname, const std::string& key);
    static void setTranslationAndRotation(Entity& entity,
                                          const tf2::Transform &tf2set);
    static geometry_msgs::TransformStamped getTransformFromChildTo(const std::string &desiredParent,
        const geometry_msgs::TransformStamped &child,
        const std::vector<geometry_msgs::TransformStamped>& tfsStamped,
        tf2::Transform& goalTransform);
    static void convert(const geometry_msgs::TransformStamped& from, tf2::Transform &to);
    static bool getParent(geometry_msgs::TransformStamped &relativeParent,
        const geometry_msgs::TransformStamped &child,
        const std::vector<geometry_msgs::TransformStamped> &transforms);
};

}; // namespace TodStandardEntities
