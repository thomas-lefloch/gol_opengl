#version 330 core

uniform vec4 is_alive;
out vec4 out_color;

void main() { out_color = is_alive; }