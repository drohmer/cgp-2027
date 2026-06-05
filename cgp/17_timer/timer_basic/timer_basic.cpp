#include "timer_basic.hpp"

#include <GLFW/glfw3.h>

namespace cgp
{

timer_basic::timer_basic()
	:t(0),scale(1.0f),running(true),time_previous(glfwGetTime())
{}
float timer_basic::update()
{
	if(!running)
        return 0.0f;

    const double time_current = glfwGetTime();
    const float dt = static_cast<float>(scale*(time_current-time_previous));

    time_previous = time_current;
    t += dt;

    return dt;
}
void timer_basic::start()
{
    running = true;
    time_previous = glfwGetTime();
}
void timer_basic::stop()
{
    running = false;
}


}