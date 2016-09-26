#version 410

in vec4 quad_color;

out vec4 frag_color;

void main () {
	frag_color = quad_color;
}