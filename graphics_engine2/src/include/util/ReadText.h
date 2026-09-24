#pragma once

#include <iostream>

#include <fstream>
#include <sstream>

namespace file {
	std::string get_text(const std::string& path) {
		std::ifstream file(path);

		if (!file.is_open()) {
			std::cout << "Failed to find or open file. (" << path << ")\n";

			return "";
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		return buffer.str();
	}
}