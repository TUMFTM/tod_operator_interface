// Copyright 2020 TUMFTM
#include "Camera.h"

namespace TodStandardEntities {

Entity Camera::create(std::shared_ptr<Scene> scene, std::string name, Entity parent) {
    Entity camera = scene->CreateEntity(name);
    camera.AddComponent<CameraComponent>(glm::perspective(glm::radians(45.0f),
                                                          (float)1280/(float)720, 0.1f, 50.0f), true);
    camera.GetComponent<TransformComponent>().setParent(parent);
    return camera;
}

}; // namespace TodStandardEntities
