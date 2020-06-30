#version 330 core

// the position variable has attribute position 0
layout(location = 0) in vec3 pos;
uniform vec2 offset;
out vec3 col;

void main() {
  vec2 offset_pos = pos.xy + offset.xy;
  gl_Position = vec4(offset_pos, pos.z, 1.0);
  col = vec3(offset_pos, .5);
}