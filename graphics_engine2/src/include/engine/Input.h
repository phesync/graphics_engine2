#pragma once

#include <unordered_set>

enum class Key {
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, ZERO,

	ESCAPE,
	SPACE,
	LEFT_SHIFT, RIGHT_SHIFT,
	LEFT_CTRL, RIGHT_CTRL,
	LEFT_ALT, RIGHT_ALT,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,

	UNKNOWN
};

class Input {
private:
	std::unordered_set<Key> pressing;
	std::unordered_set<Key> pressed;
	std::unordered_set<Key> released;
public:
	double previous_mouse_x = 0;
	double previous_mouse_y = 0;

	double mouse_x = 0;
	double mouse_y = 0;

	double delta_mouse_x = 0;
	double delta_mouse_y = 0;

	void press(const Key key) {
		pressing.insert(key);
		pressed.insert(key);
	}

	void release(const Key key) {
		pressing.erase(key);
		released.insert(key);
	}

	bool is_pressing(const Key key) const {
		return pressing.contains(key);
	}

	bool is_pressed_this_frame(const Key key) const {
		return pressed.contains(key);
	}

	bool is_released_this_frame(const Key key) const {
		return released.contains(key);
	}

	void new_poll() {
		delta_mouse_x = mouse_x - previous_mouse_x;
		delta_mouse_y = mouse_y - previous_mouse_y;

		previous_mouse_x = mouse_x;
		previous_mouse_y = mouse_y;

		pressed.clear();
		released.clear();
	}
};