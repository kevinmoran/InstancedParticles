#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "time.h"

#include "init_gl.h"
#include "maths_funcs.h"
#include "GPU_Particles.h"

GLFWwindow* window = NULL;
int gl_width = 360;
int gl_height = 240;
float gl_aspect_ratio = (float)gl_width/gl_height;

int main() {
	if (!init_gl(window, gl_width, gl_height)){ return 1; }

	srand(time(0));
	init_particle_system();

	double curr_time = glfwGetTime(), prev_time, dt;
	while (!glfwWindowShouldClose(window)) {
		//Get dt
		prev_time = curr_time;
		curr_time = glfwGetTime();
		dt = curr_time - prev_time;
		if (dt > 0.1) dt = 0.1;

		//Get Input
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		static bool enter_was_pressed = false; //wait for release
		if (glfwGetKey(window, GLFW_KEY_ENTER)) {
			if(!enter_was_pressed)
				create_particles(vec2(rand_betweenf(-0.5f, 0.5f),rand_betweenf(-0.5f, 0.5f)));
			enter_was_pressed = true;
		}
		else enter_was_pressed = false;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Draw instanced particles
		draw_particles(dt);
		glfwSwapBuffers(window);

	}//end main loop

	return 0;
}
