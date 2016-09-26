#version 410

layout (location = 0) in vec2 position;

out vec4 quad_color;

uniform vec2 offsets[100];

void main(){
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(position + offset, 0.0, 1.0);
    quad_color = vec4(offset.y, offset.x, 0.0, 1.0);
}