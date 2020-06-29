#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <helpers/RootDir.h>

#include "shader.hpp"

#define SQUARE_SIDE 18
#define SQUARE_GUTTER 2

void framebufferSizeCallback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

const int window_width = 800;
const int window_height = 600;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Game of life", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, window_width, window_height);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  const int squares_per_line = window_width / (SQUARE_SIDE + SQUARE_GUTTER);
  const int squares_per_column = window_height / (SQUARE_SIDE + SQUARE_GUTTER);

  float vertices[squares_per_line * squares_per_column * 3 * 4];
  unsigned int indices[squares_per_line * squares_per_column * 6];

  int last_vertices_idx = 0;
  int last_indices_idx = 0;
  int registered_vertices = 0;

  for (int row = -squares_per_line / 2; row < squares_per_line / 2; row++) {
    for (int col = squares_per_column / 2; col > -squares_per_column / 2; col--) {
      const float x = row * (SQUARE_SIDE + SQUARE_GUTTER);
      const float y = col * (SQUARE_SIDE + SQUARE_GUTTER);

      const float x_left = x / (window_width / 2);
      const float x_right = (x + SQUARE_SIDE) / (window_width / 2);
      const float y_top = y / (window_height / 2);
      const float y_bottom = (y - SQUARE_SIDE) / (window_height / 2);

      float top_left[3] = {x_left, y_top, 0.0};
      float top_right[3] = {x_right, y_top, 0.0};
      float bottom_left[3] = {x_left, y_bottom, 0.0};
      float bottom_right[3] = {x_right, y_bottom, 0.0};

      for (int i = 0; i < 3; i++) {
        vertices[last_vertices_idx + i] = top_left[i];
        vertices[last_vertices_idx + i + 3] = top_right[i];
        vertices[last_vertices_idx + i + 6] = bottom_left[i];
        vertices[last_vertices_idx + i + 9] = bottom_right[i];
      }

      indices[last_indices_idx] = registered_vertices;
      indices[last_indices_idx + 1] = registered_vertices + 1;
      indices[last_indices_idx + 2] = registered_vertices + 2;
      indices[last_indices_idx + 3] = registered_vertices + 1;
      indices[last_indices_idx + 4] = registered_vertices + 2;
      indices[last_indices_idx + 5] = registered_vertices + 3;

      registered_vertices += 4;
      last_indices_idx += 6;
      last_vertices_idx += 12;
    }
  }

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  // vertices location
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  int shader_id = create_shader_program(ROOT_DIR "shaders/vertex.vs", ROOT_DIR "shaders/fragment.fs");
  if (shader_id == -1) std::cout << "Error while parsing/compiling shaders" << std::endl;

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

    glUseProgram(shader_id);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}