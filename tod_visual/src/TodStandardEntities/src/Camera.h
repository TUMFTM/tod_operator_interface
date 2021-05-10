// Copyright 2020 TUMFTM
#pragma once
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include <string>
#include <memory>

namespace TodStandardEntities {

class Camera {
private:
    Camera() = default;

public:
    static Entity create(std::shared_ptr<Scene> scene, std::string name, Entity parent);
};

}; // namespace TodStandardEntities
