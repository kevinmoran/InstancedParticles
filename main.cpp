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

	//Generate instance offsets
	#define NUM_INSTANCES 100
	// vec2 instance_offsets[NUM_INSTANCES];
	srand(time(0));
	// for(int i=0; i<NUM_INSTANCES; i++){
	// 	instance_offsets[i] = vec2(rand_betweenf(-1,1), rand_betweenf(-1,1));
	// }

	//Generate instance velocities
	vec2 instance_vels[NUM_INSTANCES];
	for(int i=0; i<NUM_INSTANCES; i++){
		instance_vels[i] = vec2(rand_betweenf(-3,3), rand_betweenf(0,6));
	}

	//Generate instance colours
	vec4 instance_colours[NUM_INSTANCES];
	for(int i=0; i<NUM_INSTANCES; i++){
		instance_colours[i] = vec4(rand_betweenf(0.5f,1), rand_betweenf(0.3f,1), rand_betweenf(0.5f,0.8f), 1);
	}

	//Vertex points vbo
	GLuint particle_points_vbo;
	glGenBuffers(1, &particle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_points), particle_points, GL_STATIC_DRAW);

	//Instance offsets vbo
	// GLuint instance_vbo;
	// glGenBuffers(1, &instance_vbo);
	// glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(instance_offsets), instance_offsets, GL_STATIC_DRAW);

	//Instance velocities vbo
	GLuint instance_vel_vbo;
	glGenBuffers(1, &instance_vel_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vel_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(instance_vels), instance_vels, GL_STATIC_DRAW);
	//Instance colours vbo
	GLuint instance_colour_vbo;
	glGenBuffers(1, &instance_colour_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_colour_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(instance_colours), instance_colours, GL_STATIC_DRAW);

	//Generate VAO
	GLuint particle_vao;
	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);

	//Bind vertex points vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// //Bind instance offsets vbo
	// glEnableVertexAttribArray(1);
	// glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	// glVertexAttribDivisor(1, 1); //2nd arg = 1 means updated attribute per instance, not per vertex

	//Bind instance velocities vbo
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vel_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(1, 1); //2nd arg = 1 means updated attribute per instance, not per vertex

	//Bind instance colours vbo
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instance_colour_vbo);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(2, 1); //2nd arg = 1 means updated attribute per instance, not per vertex

	//Load shader
	Shader particle_shader("particles_instanced.vert", "pass.frag");
	glUseProgram(particle_shader.prog_id);
	GLuint origin_loc = glGetUniformLocation(particle_shader.prog_id, "origin");
	GLuint dt_loc = glGetUniformLocation(particle_shader.prog_id, "dt");
	glUniform2f(origin_loc, 0, 0);
	double particle_timer = 0.0;

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
		particle_timer += dt;
		if(particle_timer>1.5f){
			particle_timer = 0.0f;
			glUniform2f(origin_loc, rand_betweenf(-0.5f,0.5f), rand_betweenf(-0.1f, 0.1f));
		}
		glUniform1f(dt_loc, particle_timer);
		glBindVertexArray(particle_vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NUM_INSTANCES);

		glfwSwapBuffers(window);

	}//end main loop

	return 0;
}
