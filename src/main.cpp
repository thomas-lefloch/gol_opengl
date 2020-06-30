#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <helpers/RootDir.h>

#include "shader.hpp"

#define SQUARE_SIDE 10
#define SQUARE_GUTTER 1

void framebufferSizeCallback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

const int window_width = 1602;
const int window_height = 902;

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

  const int squares_per_line = (window_width - SQUARE_GUTTER) / (SQUARE_SIDE + SQUARE_GUTTER);
  const int squares_per_column = (window_height - SQUARE_GUTTER) / (SQUARE_SIDE + SQUARE_GUTTER);

  const float semi_width = SQUARE_SIDE / 2;
  const float semi_height = SQUARE_SIDE / 2;

  float vertices[] = {
      -semi_width / (window_width / 2), semi_height / (window_height / 2),  0.0f,  // top left
      semi_width / (window_width / 2),  semi_height / (window_height / 2),  0.0f,  // top right
      -semi_width / (window_width / 2), -semi_height / (window_height / 2), 0.0f,  // bottom left
      semi_width / (window_width / 2),  -semi_height / (window_height / 2), 0.0f,  // bottom right
  };

  unsigned int indices[] = {0, 1, 2, 1, 2, 3};

  bool cells[squares_per_line][squares_per_column];
  for (int row = 0; row < squares_per_line; row++) {
    for (int col = 0; col < squares_per_column; col++) {
      cells[row][col] = (row + col) % 2;
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

    int is_alive_loc = glGetUniformLocation(shader_id, "is_alive");
    int offset_loc = glGetUniformLocation(shader_id, "offset");

    for (int row = -squares_per_line / 2; row < squares_per_line / 2; row++) {
      for (int col = -squares_per_column / 2; col < squares_per_column / 2; col++) {
        glUniform1i(is_alive_loc, cells[row + squares_per_line / 2][col + squares_per_column / 2]);
        const float offset_x = (SQUARE_SIDE + SQUARE_GUTTER) * row + (SQUARE_SIDE / 2);
        const float offset_y = (SQUARE_SIDE + SQUARE_GUTTER) * col + (SQUARE_SIDE / 2);
        glUniform2f(offset_loc, offset_x / (window_width / 2), offset_y / (window_height / 2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
    glBindVertexArray(0);
  }
}