// Copyright 2021 TUMFTM
#pragma once
static const float PI{3.14159f};
#include <memory>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <limits>
#include <future>
#include <map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Core/Application.h"
#include "Core/CursorPosition.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "TodStandardEntities/TodStandardEntities.h"

class Interface : public Application {
public:
    Interface(int argc, char **argv);

private:
    void CreateScene();
    void CreateCosysEntities(Entity &CosysOrigin, Entity &CosysBaseFootprint,
                             std::map<std::string, Entity> &mapOfRelativeCosysEntities);
    void CreateDisplayEntities(const Entity &CosysBaseFootprint);
    void CreateGridAndFloorEntities(const Entity &CosysBaseFootprint);
    void CreateCameraFramebufferAndTopViewEntities(const Entity &CosysBaseFootprint);
    void CreateVideoEntities(const Entity &CosysBaseFootprint);
    void CreateLaneEntities(const Entity &CosysBaseFootprint);
    void CreateLaserScanEntities(const std::map<std::string, Entity> &mapOfRelativeCosysEntities);
    bool DeserializeEntityData(const std::string &vehicleID);
    void CreateVideoMeshes(const bool couldDeserialize, const std::string &vehicleID);
};
