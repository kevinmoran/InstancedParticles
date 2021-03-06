#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 vel;
layout (location = 2) in vec4 colour; //colour.a is lifetime
layout (location = 3) in float timer;
layout (location = 4) in vec2 block_origin;

//uniform vec2 origin; //particle source position
float lifetime = 1.4; //how long particles live for
//uniform float dt;
const float g = -1.81;

out vec4 quad_colour;

void main(){
    //Simulate particle movement
    // MAD (MUL_ADD) is only one instruction! Cool!
    // https://www.opengl.org/wiki/GLSL_Optimizations#Get_MAD
    float new_vel_y = vel.y + g*timer;
    vec2 curr_vel = vec2(vel.x, new_vel_y);
    vec2 curr_pos = block_origin + curr_vel*timer;
    gl_Position = vec4(position+curr_pos, 0.0, 1.0);
    //gl_Position = vec4(block_origin, 0.0, 1.0);

    //Fade out particle as it expires
    quad_colour = colour;
    quad_colour.a = (lifetime-timer)*quad_colour.a;
}