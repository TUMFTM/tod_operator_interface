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
#include "Core/ModelLoader.h"
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
    void CreateCosysEntities(std::map<std::string, Entity> &coordinateSystems);
    void CreateDisplayEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateVehicleModelEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateGridAndFloorEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateCameraFramebufferAndTopViewEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateVideoEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateLaneEntities(const std::map<std::string, Entity> &coordinateSystems);
    void CreateLaserScanEntities(const std::map<std::string, Entity> &coordinateSystems);
    bool DeserializeEntityData(const std::string &vehicleID);
    void CreateVideoMeshes(const bool couldDeserialize, const std::string &vehicleID);
};
