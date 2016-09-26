#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "time.h"

#include "init_gl.h"
#include "maths_funcs.h"
#include "Shader.h"
#include "rand_utils.h"

GLFWwindow* window = NULL;
int gl_width = 360;
int gl_height = 240;
float gl_aspect_ratio = (float)gl_width/gl_height;

int main() {
	if (!init_gl(window, gl_width, gl_height)){ return 1; }

	//Geometry setup
	GLfloat particle_points[] = {
		-0.05f,	0.05f,
		-0.05f, -0.05f,
		 0.05f, -0.05f,
		-0.05f,	0.05f,
		 0.05f, -0.05f,
		 0.05f,  0.05f
	};

	GLuint particle_vao;
	GLuint particle_points_vbo;
	glGenBuffers(1, &particle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_points), particle_points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Load shader
	Shader particle_shader("particles_instanced.vert", "pass.frag");
	glUseProgram(particle_shader.prog_id);

	vec2 instance_offsets[100];
	srand(time(0));

	for(int i=0; i<100; i++){
		instance_offsets[i] = vec2(rand_betweenf(-1,1), rand_betweenf(-1,1));
		char offset_string[32];
		sprintf(offset_string, "offsets[%i]", i);
		GLint location = glGetUniformLocation(particle_shader.prog_id, offset_string);
		glUniform2fv(location, 1, instance_offsets[i].v);
	}

	double curr_time = glfwGetTime(), prev_time, dt;
	//-------------------------------------------------------------------------------------//
	//-------------------------------------MAIN LOOP---------------------------------------//
	//-------------------------------------------------------------------------------------//
	while (!glfwWindowShouldClose(window)) {
		//Get dt
		prev_time = curr_time;
		curr_time = glfwGetTime();
		dt = curr_time - prev_time;
		if (dt > 0.1) dt = 0.1;

		// static float fps_timer = 0.0f;
		// fps_timer+=dt;
		// if(fps_timer>0.2f){
		// 	char title_string[64];
		// 	sprintf(title_string, "Wander - %.2fHz", 1/dt);
		// 	glfwSetWindowTitle(window, title_string);
		// 	fps_timer = 0.0f;
		// }

		//Get Input
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		//Update game state

		//Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw instanced particles
		glUseProgram(particle_shader.prog_id);
		glBindVertexArray(particle_vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

		glfwSwapBuffers(window);

	}//end main loop

	return 0;
}
