#version 330 core

in vec3 col;
uniform int is_alive;
out vec4 out_color;

void main() {
  if (is_alive == 1) {
    out_color = vec4(0, 0, 0, 0);
  } else {
    out_color = vec4(1.0, 1.0, 1.0, 1.0);
  }
}