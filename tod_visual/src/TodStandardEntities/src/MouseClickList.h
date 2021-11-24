// Copyright 2021 Feiler
// PERCEPTION MODIFICATION
#pragma once
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Systems/ShaderSystem.h"
#include "sensor_msgs/PointCloud.h"
#include <memory>
#include <string>

namespace TodStandardEntities {

class MouseClickList {
    private:
        MouseClickList() = default;

    public:
        static Entity create(std::shared_ptr<Scene> scene,
            std::string name, Entity parent, const std::string& packagePath);
        static void onMouseClickListReceived(const sensor_msgs::PointCloudConstPtr& points,
            Entity &entity);
};

}   // namespace TodStandardEntities
