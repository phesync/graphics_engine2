#pragma once

#include <data/Plane.h>
#include <data/Camera.h>

struct Frustum
{
    Plane top;
    Plane bottom;

    Plane right;
    Plane left;

    Plane far;
    Plane near;

    Frustum(const Camera& camera) {
        double far_plane_height = camera.far_plane * tanf(camera.fov);
        double far_plane_width = far_plane_height * camera.aspect;

        float far_half_width = far_plane_width / 2;
        float far_half_height = far_plane_height / 2;

        glm::vec3 t_forward = camera.transform.forward();
        glm::vec3 t_right = camera.transform.right();
        glm::vec3 t_up = camera.transform.up();

        glm::vec3 far_center = camera.far_plane * t_forward;

        glm::vec3 near_point = camera.transform.pos + (camera.near_plane * t_forward);
        glm::vec3 far_point = camera.transform.pos + far_center;

        near.normal = t_forward;
        near.distance = glm::dot(near.normal, near_point);

        far.normal = -t_forward;
        far.distance = glm::dot(far.normal, far_point);

        right.normal = -glm::cross(far_center - (t_right * far_half_width), t_up);
        right.distance = glm::dot(right.normal, camera.transform.pos);

        left.normal = glm::cross(far_center + (t_right * far_half_width), t_up);
        left.distance = glm::dot(left.normal, camera.transform.pos);

        top.normal = glm::cross(far_center - (t_up * far_half_height), t_right);
        top.distance = glm::dot(top.normal, camera.transform.pos);

        bottom.normal = -glm::cross(far_center + (t_up * far_half_height), t_right);
        bottom.distance = glm::dot(bottom.normal, camera.transform.pos);
    }
};