// Copyright 2020 TUMFTM
#include "CoordinateSystem.h"

namespace TodStandardEntities {

Entity CoordinateSystem::create(
    std::shared_ptr<Scene> scene, std::string name, const std::string &packagePath) {
    Entity coordinateSystem = scene->CreateEntity(name);
    unsigned int grid_shader = ShaderSystem::createShaderProgram(
        (packagePath + "/resources/OpenGL/shaders/shader.vert").c_str(),
        (packagePath + "/resources/OpenGL/shaders/shader.frag").c_str());
    std::vector<Vertex> vertices;
    vertices.emplace_back(glm::vec3(0.00f, 0.0f, 0.0f), glm::vec2(), glm::vec3(1.0f, 0.0f, 0.0f));
    vertices.emplace_back(glm::vec3(0.25f, 0.0f, 0.0f), glm::vec2(), glm::vec3(1.0f, 0.0f, 0.0f));
    vertices.emplace_back(glm::vec3(0.0f, 0.00f, 0.0f), glm::vec2(), glm::vec3(0.0f, 1.0f, 0.0f));
    vertices.emplace_back(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec2(), glm::vec3(0.0f, 1.0f, 0.0f));
    vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.00f), glm::vec2(), glm::vec3(0.0f, 0.0f, 1.0f));
    vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.25f), glm::vec2(), glm::vec3(0.0f, 0.0f, 1.0f));
    coordinateSystem.AddComponent<RenderableElementComponent>(grid_shader, Mesh(vertices), GL_LINES);
    coordinateSystem.GetComponent<RenderableElementComponent>().LineWidth = 5.0f;
    return coordinateSystem;
}

std::map<std::string, Entity> CoordinateSystem::createTransformTree(
    std::shared_ptr<Scene> scene, const std::string &packagePath, Entity& cosysBaseFootPrint) {
    std::string parent{"base_footprint"};
    checkIfTagMatchesKey(cosysBaseFootPrint, parent);

    ros::NodeHandle nodeHandle("~");
    std::vector<geometry_msgs::TransformStamped> tfsStamped =
        CommonTransformTreePublisher::getTransformTreeFromParamServer(
            "/Operator/Transform/CommonTransformTreePublisher", nodeHandle);

    std::map<std::string, Entity> mapOfEntities;
    for ( auto& childTfStamped : tfsStamped ) {
        tf2::Transform goalTf;
        convert(childTfStamped, goalTf);
        geometry_msgs::TransformStamped tf2parent = getTransformFromChildTo(parent, childTfStamped, tfsStamped, goalTf);
        if ( tf2parent.header.frame_id == parent ) {
            std::string entityName { "Cosys" + childTfStamped.child_frame_id };
            Entity newCosys = TodStandardEntities::CoordinateSystem::create(scene, entityName, packagePath);
            mapOfEntities.insert_or_assign(entityName, newCosys);
            setTranslationAndRotation(newCosys, goalTf);
            newCosys.GetComponent<TransformComponent>().setParent(cosysBaseFootPrint);
            newCosys.GetComponent<RenderableElementComponent>().StaticShow = false;
        }
    }
    return mapOfEntities;
}

void CoordinateSystem::checkIfTagMatchesKey(Entity &entity, const std::string& key) {
    if (entity.GetComponent<TagComponent>().Tag != key)
        ROS_ERROR("tofGL::CoordinateSystem - entity.Tag = %s does not match key %s",
                  entity.GetComponent<TagComponent>().Tag.c_str(), key.c_str());
}

void CoordinateSystem::convert(const geometry_msgs::TransformStamped& from, tf2::Transform &to) {
    auto &tf = from.transform;
    to.setOrigin(tf2::Vector3((double) tf.translation.x, (double) tf.translation.y, (double) tf.translation.z));
    to.setRotation(tf2::Quaternion(tf.rotation.x, tf.rotation.y, tf.rotation.z, tf.rotation.w));
}

geometry_msgs::TransformStamped CoordinateSystem::getTransformFromChildTo(
    const std::string &desiredParent, const geometry_msgs::TransformStamped &child,
    const std::vector<geometry_msgs::TransformStamped> &tfsStamped,
    tf2::Transform &goalTransform) {
    // recurse until parent is found
    if ( child.header.frame_id == desiredParent ) {
        return child;
    }
    geometry_msgs::TransformStamped tf2parent;
    if ( getParent(tf2parent, child, tfsStamped) ) {
        tf2::Transform missingRelativeTransform;
        convert(tf2parent, missingRelativeTransform);
        tf2::Transform tfFromParentToChild(goalTransform);
        goalTransform.mult(missingRelativeTransform, tfFromParentToChild);
        return getTransformFromChildTo(desiredParent, tf2parent, tfsStamped, goalTransform);
    } else {
        // finished
        return child;
    }
}

bool CoordinateSystem::getParent(geometry_msgs::TransformStamped& relativeParent,
                                 const geometry_msgs::TransformStamped& child,
                                 const std::vector<geometry_msgs::TransformStamped>& transforms) {
    for ( auto& elem : transforms ) {
        if ( elem.child_frame_id == child.header.frame_id ) {
            relativeParent = elem;
            return true;
        }
    }
    return false;
}

void CoordinateSystem::setTranslationAndRotation(Entity& entity, const tf2::Transform &tf2set) {
    auto &tf = entity.GetComponent<TransformComponent>();
    const auto &tx = tf2set.getOrigin();
    tf.setTranslation(glm::vec3(tx.getX(), tx.getY(), tx.getZ()));
    const auto &rx = tf2set.getRotation();
    tf2::Quaternion quat(rx.getX(), rx.getY(), rx.getZ(), rx.getW());
    tf2::Matrix3x3 eulerMatrix(quat);
    double yaw, pitch, roll;
    eulerMatrix.getEulerYPR(yaw, pitch, roll);
    entity.GetComponent<TransformComponent>().setRotation(
        glm::vec3((float) roll, (float) pitch, (float) yaw));
}

} // namespace TodStandardEntities
