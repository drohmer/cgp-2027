#include "cgp/01_base/base.hpp"
#include "../camera_controller.hpp"


#include <iostream>

#if defined(__linux__) || defined(__EMSCRIPTEN__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace cgp_test 
{

	void test_camera_controller()
	{
		{
			cgp::camera_controller_orbit controller;
			controller.look_at({ 1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg( is_equal(controller.camera_model.front(), {-1, 0, 0}) );
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 0, 0 }));

			controller.look_at({ -1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { -1, 0, 0 }));

			controller.look_at({ 0,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 0, -1, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 0, 1, 0 }));

			float const sqrt2 = std::sqrt(2.0f);
			controller.look_at({ 1,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1/sqrt2, -1/sqrt2, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 1, 0 }));
		}

		{
			cgp::camera_controller_orbit_euler controller;
			controller.look_at({ 1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 0, 0 }));

			controller.look_at({ -1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { -1, 0, 0 }));

			controller.look_at({ 0,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 0, -1, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 0, 1, 0 }));

			float const sqrt2 = std::sqrt(2.0f);
			controller.look_at({ 1,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1 / sqrt2, -1 / sqrt2, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 1, 0 }));
		}

		{
			cgp::camera_controller_first_person controller;
			controller.look_at({ 1, 0, 0 }, {0,0,0}, {0,0,1});
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 0, 0 }));

			controller.look_at({ -1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { -1, 0, 0 }));

			controller.look_at({ 0,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 0, -1, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 0, 1, 0 }));

			float const sqrt2 = std::sqrt(2.0f);
			controller.look_at({ 1,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1 / sqrt2, -1 / sqrt2, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 1, 0 }));
		}

		{
			cgp::camera_controller_first_person_euler controller;
			controller.look_at({ 1, 0, 0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 0, 0 }));

			controller.look_at({ -1,0,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 1, 0, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { -1, 0, 0 }));

			controller.look_at({ 0,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { 0, -1, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 0, 1, 0 }));

			float const sqrt2 = std::sqrt(2.0f);
			controller.look_at({ 1,1,0 }, { 0,0,0 }, { 0,0,1 });
			assert_cgp_no_msg(is_equal(controller.camera_model.front(), { -1.0f / sqrt2, -1.0f / sqrt2, 0 }));
			assert_cgp_no_msg(is_equal(controller.camera_model.position(), { 1, 1, 0 }));
		}



		// fly mode: holding R for one second gives the same speed at 60 and at 144 fps
		{
			auto speed_after_one_second = [](int fps) {
				cgp::input_devices inputs;
				cgp::window_structure window;
				cgp::camera_controller_fly_mode controller;
				controller.inputs = &inputs;
				controller.window = &window;
				inputs.keyboard.update_from_glfw_key(GLFW_KEY_R, GLFW_PRESS);
				inputs.time_interval = 1.0f / fps;
				for (int k = 0; k < fps; ++k)
					controller.idle_frame();
				return controller.speed;
			};
			float const s60 = speed_after_one_second(60);
			float const s144 = speed_after_one_second(144);
			assert_cgp_no_msg(std::abs(s60 - 0.05f * std::pow(1.02f, 60.0f)) < 1e-4f); // unchanged at 60 fps
			assert_cgp_no_msg(std::abs(s144 - s60) < 1e-4f);
		}
	}
}