#pragma once

#include <data/ObjectTransform.h>

class Camera {
public:
    ObjTransform transform;

    float fov;

    float near_plane;
    float far_plane;

    float aspect;

    glm::mat4x4 get_perspective() const {
        return glm::perspective(
            glm::radians(fov),
            aspect,
            near_plane,
            far_plane
        );
    }

    glm::mat4x4 get_view() const {
        return glm::inverse(transform.get_matrix());
    }

    Camera(float c_fov) : 
        fov(c_fov),
        near_plane(0.1),
        far_plane(1000),
        aspect(1)
    {};
};