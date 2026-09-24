#pragma once

namespace map {
	template <typename Map, typename Key>
	typename Map::mapped_type* get_ptr(Map& map, const Key& key) {
		auto it = map.find(key);

		return it == map.end() ? nullptr : &it->second;
	}

	template <typename Map, typename Key>
	const typename Map::mapped_type* get_ptr(const Map& map, const Key& key) {
		auto it = map.find(key);

		return it == map.end() ? nullptr : &it->second;
	}
}