#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 offset;

out vec4 quad_color;

void main(){
    gl_Position = vec4(position + offset, 0.0, 1.0);
    quad_color = vec4(offset.y, offset.x, 0.0, 1.0);
}