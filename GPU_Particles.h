#pragma once
#include <GL/glew.h>
#include "Shader.h"
#include "maths_funcs.h"
#include "rand_utils.h"

//Internal system data
#define MAX_NUM_PARTICLES 1024
#define PARTICLE_BLOCK_SIZE 64
#define NUM_PARTICLE_BLOCKS (MAX_NUM_PARTICLES/PARTICLE_BLOCK_SIZE)
static unsigned int num_live_particles = 0;
static unsigned int num_live_blocks = 0;
static float timers[NUM_PARTICLE_BLOCKS] = {};
static vec2 origins[NUM_PARTICLE_BLOCKS] = {};

Shader particle_system_shader;
GLuint origin_loc, dt_loc;

static GLuint particle_vao;
static GLuint particle_points_vbo;
static GLuint instance_vel_vbo;
static GLuint instance_colour_vbo;
static GLuint timers_vbo;
static GLuint origins_vbo;

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

	//Vertex points vbo
	glGenBuffers(1, &particle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_points), particle_points, GL_STATIC_DRAW);

	// *** Per-instance data *** //
	//Instance velocities vbo
	glGenBuffers(1, &instance_vel_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vel_vbo);
	//Reserve an empty buffer of the desired size
	glBufferData(GL_ARRAY_BUFFER, MAX_NUM_PARTICLES*sizeof(vec2), NULL, GL_STATIC_DRAW);

	//Instance colours vbo
	glGenBuffers(1, &instance_colour_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_colour_vbo);
	//Reserve an empty buffer of the desired size 
	glBufferData(GL_ARRAY_BUFFER, MAX_NUM_PARTICLES*sizeof(vec4), NULL, GL_STATIC_DRAW);

    // *** Per-block data *** //
    //Block timers vbo
    glGenBuffers(1, &timers_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, timers_vbo);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLE_BLOCKS*sizeof(float), NULL, GL_STATIC_DRAW);

    //Block origins vbo
    glGenBuffers(1, &origins_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, origins_vbo);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLE_BLOCKS*sizeof(vec2), NULL, GL_STATIC_DRAW);

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

    //Bind timers vbo
    glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, timers_vbo);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(3, PARTICLE_BLOCK_SIZE);

    //Bind origins vbo
    glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, origins_vbo);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(4, PARTICLE_BLOCK_SIZE);

    //Load shader
    particle_system_shader.load_shader_program("particles_instanced.vert", "pass.frag");
    glUseProgram(particle_system_shader.prog_id);
	//origin_loc = glGetUniformLocation(particle_system_shader.prog_id, "origin");
	//dt_loc = glGetUniformLocation(particle_system_shader.prog_id, "dt");
	//glUniform2f(origin_loc, 0, 0);
	
}

void draw_particles(double dt){
    for(int i=0; i<num_live_blocks; i++){
        timers[i] += dt;
        if(timers[i]>1.4f) {
            timers[i] = 0.0f;
            num_live_particles -= PARTICLE_BLOCK_SIZE;
            num_live_blocks--;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, timers_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (num_live_blocks)*sizeof(float), timers);
    
    glUseProgram(particle_system_shader.prog_id);
	//particle_timer += dt;
    //glUniform1f(dt_loc, particle_timer);
    glBindVertexArray(particle_vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_live_particles);
}

void create_particles(vec2 location){
    //Generate data for new particle effect
	vec2 instance_vels[PARTICLE_BLOCK_SIZE];
	vec4 instance_colours[PARTICLE_BLOCK_SIZE];

    //Generate instance velocities
	for(int i=0; i<PARTICLE_BLOCK_SIZE; i++){
		//Generate random angle
		float theta = rand_betweenf(0, 2.0*M_PI);
		//rotate (1,0) by theta
		vec2 rand_dir = vec2(cos(theta), sin(theta));
		instance_vels[i] = rand_dir * rand_betweenf(0.5, 2); //scale velocity randomly
	}

	//Generate instance colours
	for(int i=0; i<PARTICLE_BLOCK_SIZE; i++){
		instance_colours[i] = vec4(rand_betweenf(0.5f,1), rand_betweenf(0.3f,1), rand_betweenf(0.5f,0.8f), 1);
	}
    
	//Send over velocity data
    glBindBuffer(GL_ARRAY_BUFFER, instance_vel_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, num_live_particles*sizeof(vec2), sizeof(instance_vels), instance_vels);
	//Send over colour data
    glBindBuffer(GL_ARRAY_BUFFER, instance_colour_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, num_live_particles*sizeof(vec4), sizeof(instance_colours), instance_colours);
    //Send over block's timer data
    timers[num_live_blocks] = 0.0f;
    glBindBuffer(GL_ARRAY_BUFFER, timers_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, num_live_blocks*sizeof(float), sizeof(float), &timers[num_live_blocks]);
    //Send over block's origin
    origins[num_live_blocks] = location;
    glBindBuffer(GL_ARRAY_BUFFER, origins_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, num_live_blocks*sizeof(vec2), sizeof(vec2), &origins[num_live_blocks]);
    
    num_live_particles += PARTICLE_BLOCK_SIZE;
    num_live_blocks++;
    //particle_timer = 0.0;
    //glUniform2fv(origin_loc, 1, location.v);
}
