#pragma once

#include <glm/vec3.hpp>

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane() :
        normal({ 0, 1, 0 }),
        distance(0)
    {};
};