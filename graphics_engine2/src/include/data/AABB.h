#pragma once

#include <glm/vec3.hpp>

#include <data/Frustum.h>
#include <data/Plane.h>
#include <data/Ray.h>

//constexpr float EPSILON = 0.0000001;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	bool test_aabb(const AABB& aabb) const {
		return (aabb.min.x <= max.x && aabb.min.y <= max.y && aabb.min.z <= max.z && aabb.max.x >= min.x && aabb.max.y >= min.y && aabb.max.z >= min.z);
	}

	bool test_plane(const Plane& plane) const {
		glm::vec3 point;

		point.x = plane.normal.x >= 0 ? max.x : min.x;
		point.y = plane.normal.y >= 0 ? max.y : min.y;
		point.z = plane.normal.z >= 0 ? max.z : min.z;

		return glm::dot(plane.normal, point) > plane.distance;
	}

	bool test_frustum(const Frustum& frustum) const {
		return test_plane(frustum.near) && test_plane(frustum.far) && test_plane(frustum.left) && test_plane(frustum.right) && test_plane(frustum.top) && test_plane(frustum.bottom);
	}

	bool test_ray(const Ray& ray) const {
		if (ray.direction.x == 0 || ray.direction.y == 0 || ray.direction.z == 0) {
			return false;
		}

		float txmin = (min.x - ray.origin.x) / ray.direction.x;
		float tymin = (min.y - ray.origin.y) / ray.direction.y;
		float tzmin = (min.z - ray.origin.z) / ray.direction.z;

		float txmax = (max.x - ray.origin.x) / ray.direction.x;
		float tymax = (max.y - ray.origin.y) / ray.direction.y;
		float tzmax = (max.z - ray.origin.z) / ray.direction.z;

		float txenter = std::min(txmin, txmax);
		float tyenter = std::min(tymin, tymax);
		float tzenter = std::min(tzmin, tzmax);

		float txexit = std::max(txmin, txmax);
		float tyexit = std::max(tymin, tymax);
		float tzexit = std::max(tzmin, tzmax);

		float tenter = std::max({ txenter, tyenter, tzenter });
		float texit = std::min({ txexit, tyexit, tzexit });

		return tenter <= texit;
	};

	static AABB from_transform(glm::vec3 pos, glm::vec3 scale) {
		AABB new_aabb;

		float half_x = scale.x / 2;
		float half_y = scale.y / 2;
		float half_z = scale.z / 2;

		new_aabb.min.x = pos.x - half_x;
		new_aabb.min.y = pos.y - half_y;
		new_aabb.min.z = pos.z - half_z;

		new_aabb.max.x = pos.x + half_x;
		new_aabb.max.y = pos.y + half_y;
		new_aabb.max.z = pos.z + half_z;

		return new_aabb;
	}
};