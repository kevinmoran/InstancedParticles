#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "maths_funcs.h"
#include "rand_utils.h"

//Internal system data
#define NUM_INSTANCES 500
static unsigned int num_live_particles = NUM_INSTANCES;
double particle_timer = 0.0;

Shader particle_system_shader;
GLuint origin_loc, dt_loc;

static GLuint particle_vao;
static GLuint particle_points_vbo;
static GLuint instance_vel_vbo;
static GLuint instance_colour_vbo;

void init_particle_system(){
    //Geometry setup
	GLfloat particle_points[] = {
		-0.01f,	0.01f,
		-0.01f, -0.01f,
		 0.01f, -0.01f,
		-0.01f,	0.01f,
		 0.01f, -0.01f,
		 0.01f,  0.01f
	};

	//Generate instance velocities
	vec2 instance_vels[NUM_INSTANCES];
	for(int i=0; i<NUM_INSTANCES; i++){
		//Generate random angle
		float theta = rand_betweenf(0, 2.0*M_PI);
		//rotate (1,0) by theta
		vec2 rand_dir = vec2(cos(theta), sin(theta));
		instance_vels[i] = rand_dir * rand_betweenf(0.5, 5);
	}

	//Generate instance colours
	vec4 instance_colours[NUM_INSTANCES];
	for(int i=0; i<NUM_INSTANCES; i++){
		instance_colours[i] = vec4(rand_betweenf(0.5f,1), rand_betweenf(0.3f,1), rand_betweenf(0.5f,0.8f), 1);
	}

	//Vertex points vbo
	glGenBuffers(1, &particle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_points), particle_points, GL_STATIC_DRAW);

	// *** Per-instance data *** //
	//Instance velocities vbo
	glGenBuffers(1, &instance_vel_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vel_vbo);
	//Reserve an empty buffer of the desired size
	glBufferData(GL_ARRAY_BUFFER, sizeof(instance_vels), NULL, GL_STATIC_DRAW);
	//Send over data into empty buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance_vels), instance_vels);

	//Instance colours vbo
	glGenBuffers(1, &instance_colour_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_colour_vbo);
	//Reserve an empty buffer of the desired size 
	glBufferData(GL_ARRAY_BUFFER, sizeof(instance_colours), NULL, GL_STATIC_DRAW);
	//Send over data into empty buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance_colours), instance_colours);

	//Generate VAO
	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);

	//Bind vertex points vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

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
    particle_system_shader.load_shader_program("particles_instanced.vert", "pass.frag");
    glUseProgram(particle_system_shader.prog_id);
	origin_loc = glGetUniformLocation(particle_system_shader.prog_id, "origin");
	dt_loc = glGetUniformLocation(particle_system_shader.prog_id, "dt");
	glUniform2f(origin_loc, 0, 0);
	
}

void draw_particles(double dt){
    glUseProgram(particle_system_shader.prog_id);
	particle_timer += dt;
    if(particle_timer>1.5f){
        particle_timer = 0.0f;
        glUniform2f(origin_loc, rand_betweenf(-0.5f,0.5f), rand_betweenf(-0.1f, 0.1f));
    }
    glUniform1f(dt_loc, particle_timer);
    glBindVertexArray(particle_vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_live_particles);
}
