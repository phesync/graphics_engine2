#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct ObjTransform {
    glm::vec3 pos;
    glm::vec3 scale;
    glm::quat rot;

    void set_euler(float pitch, float yaw, float roll) {
        rot = glm::quat(glm::vec3(
            glm::radians(pitch),
            glm::radians(yaw),
            glm::radians(roll)
        ));
    }

    glm::mat4 get_matrix() const {
        return glm::translate(glm::mat4(1.0f), pos) * glm::mat4(rot);
    };

    glm::vec3 forward() const {
        return rot * glm::vec3(0, 0, -1);
    }

    glm::vec3 right() const {
        return rot * glm::vec3(1, 0, 0);
    }

    glm::vec3 up() const {
        return rot * glm::vec3(0, 1, 0);
    }

    ObjTransform() : 
        pos({0, 0, 0}),
        rot({0, 0, 0}),
        scale({0, 0, 0})
    {};
};