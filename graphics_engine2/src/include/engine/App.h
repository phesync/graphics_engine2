#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <engine/Input.h>
#include <engine/Game.h>
#include <engine/Resources.h>
#include <data/WindowCommands.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

Key get_key_from_glfw(int key) {
	switch (key) {
	case GLFW_KEY_A: return Key::A;
	case GLFW_KEY_B: return Key::B;
	case GLFW_KEY_C: return Key::C;
	case GLFW_KEY_D: return Key::D;
	case GLFW_KEY_E: return Key::E;
	case GLFW_KEY_F: return Key::F;
	case GLFW_KEY_G: return Key::G;
	case GLFW_KEY_H: return Key::H;
	case GLFW_KEY_I: return Key::I;
	case GLFW_KEY_J: return Key::J;
	case GLFW_KEY_K: return Key::K;
	case GLFW_KEY_L: return Key::L;
	case GLFW_KEY_M: return Key::M;
	case GLFW_KEY_N: return Key::N;
	case GLFW_KEY_O: return Key::O;
	case GLFW_KEY_P: return Key::P;
	case GLFW_KEY_Q: return Key::Q;
	case GLFW_KEY_R: return Key::R;
	case GLFW_KEY_S: return Key::S;
	case GLFW_KEY_T: return Key::T;
	case GLFW_KEY_U: return Key::U;
	case GLFW_KEY_V: return Key::V;
	case GLFW_KEY_W: return Key::W;
	case GLFW_KEY_X: return Key::X;
	case GLFW_KEY_Y: return Key::Y;
	case GLFW_KEY_Z: return Key::Z;
	case GLFW_KEY_1: return Key::ONE;
	case GLFW_KEY_2: return Key::TWO;
	case GLFW_KEY_3: return Key::THREE;
	case GLFW_KEY_4: return Key::FOUR;
	case GLFW_KEY_5: return Key::FIVE;
	case GLFW_KEY_6: return Key::SIX;
	case GLFW_KEY_7: return Key::SEVEN;
	case GLFW_KEY_8: return Key::EIGHT;
	case GLFW_KEY_9: return Key::NINE;
	case GLFW_KEY_0: return Key::ZERO;
	case GLFW_KEY_ESCAPE: return Key::ESCAPE;
	case GLFW_KEY_SPACE: return Key::SPACE;
	case GLFW_KEY_LEFT_SHIFT: return Key::LEFT_SHIFT;
	case GLFW_KEY_RIGHT_SHIFT: return Key::RIGHT_SHIFT;
	case GLFW_KEY_LEFT_CONTROL: return Key::LEFT_CTRL;
	case GLFW_KEY_RIGHT_CONTROL: return Key::RIGHT_CTRL;
	case GLFW_KEY_LEFT_ALT: return Key::LEFT_ALT;
	case GLFW_KEY_RIGHT_ALT: return Key::RIGHT_ALT;
	case GLFW_MOUSE_BUTTON_LEFT: return Key::MOUSE_LEFT;
	case GLFW_MOUSE_BUTTON_RIGHT: return Key::MOUSE_RIGHT;
	case GLFW_MOUSE_BUTTON_MIDDLE: return Key::MOUSE_MIDDLE;
	default: return Key::UNKNOWN;
	};
};

class MainApp {
private:
	GLFWwindow* glfw_window = nullptr;

	int window_width;
	int window_height;

	Input input;
	Game* game;

	void create_window(const std::string& name, int w_width, int w_height, bool full_screened) {
		if (glfw_window != nullptr) return;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (full_screened) {
			glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		}

		glfw_window = glfwCreateWindow(w_width, w_height, name.c_str(), NULL, NULL);
	}

	// input callbacks
	void glfw_key_callback(int key, int scancode, int action, int mods) {
		Key input_key = get_key_from_glfw(key);

		if (input_key != Key::UNKNOWN) {
			if (action == GLFW_PRESS) {
				input.press(input_key);
			}
			else if (action == GLFW_RELEASE) {
				input.release(input_key);
			};
		};
	}

	void glfw_mouse_pos_callback(double x, double y) {
		input.mouse_x = x;
		input.mouse_y = y;
	}

	void glfw_mouse_button_callback(int button, int action, int mods) {
		glfw_key_callback(button, 0, action, mods);
	}
	//

	// static input callbacks
	static void static_glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		MainApp* app = (MainApp*)glfwGetWindowUserPointer(window);

		if (app) app->glfw_key_callback(key, scancode, action, mods);
	}

	static void static_glfw_mouse_pos_callback(GLFWwindow* window, double x, double y)
	{
		MainApp* app = (MainApp*)glfwGetWindowUserPointer(window);

		if (app) app->glfw_mouse_pos_callback(x, y);
	}

	static void static_glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		MainApp* app = (MainApp*)glfwGetWindowUserPointer(window);

		if (app) app->glfw_mouse_button_callback(button, action, mods);
	}
	//

	void set_window_commands(WindowCommands& commands) {
		if (commands.close) glfwSetWindowShouldClose(glfw_window, true);

		if (commands.lock_mouse) {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (commands.captured_mouse) {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
		}
		else {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void raw_update() {
		glfwGetFramebufferSize(glfw_window, &window_width, &window_height);

		input.new_poll();
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		WindowCommands commands;

		game->update(input, window_width, window_height, commands);

		set_window_commands(commands);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfw_window);
	}

public:
	void run() {
		glfwInit();

		create_window("applicat", 500, 500, true);

		glfwMakeContextCurrent(glfw_window);
		glfwSetWindowUserPointer(glfw_window, this);

		glfwSetKeyCallback(glfw_window, static_glfw_key_callback);
		glfwSetCursorPosCallback(glfw_window, static_glfw_mouse_pos_callback);
		glfwSetMouseButtonCallback(glfw_window, static_glfw_mouse_button_callback);

		glfwGetCursorPos(glfw_window, &input.mouse_x, &input.mouse_y);

		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		gladLoadGL(glfwGetProcAddress);

		// ImGui
		ImGui::CreateContext();

		ImGui::GetIO().IniFilename = nullptr;

		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL3_Init();
		//

		gpu_resources::load_all();
		
		game = new Game();

		while (!glfwWindowShouldClose(glfw_window)) {
			raw_update();
		};
	};

	~MainApp() {
		glfwDestroyWindow(glfw_window);
		glfwTerminate();

		delete game;
	}
};