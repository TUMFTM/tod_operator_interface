// Copyright 2021 Schimpe
#pragma once
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Systems/ShaderSystem.h"
#include <sensor_msgs/LaserScan.h>
#include <string>
#include <memory>

namespace TodStandardEntities {

class LaserScan {
public:
    static Entity create(std::shared_ptr<Scene> scene, const std::string &name,
                         Entity parent, const std::string& packagePath);
    static void onLaserScanReceived(const sensor_msgs::LaserScanConstPtr& msg, Entity &entity);

private:
    LaserScan() = default;
};

}; // namespace TodStandardEntities
