#version 330 core

// the position variable has attribute position 0
layout(location = 0) in vec3 pos;
uniform vec2 offset;

void main() { gl_Position = vec4(pos.xy + offset.xy, pos.z, 1.0); }