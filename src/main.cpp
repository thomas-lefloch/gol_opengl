#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>

#include <helpers/RootDir.h>

#include "shader.hpp"

double cursor_x = 0;
double cursor_y = 0;

constexpr int square_side = 10;
constexpr int square_gutter = 1;

constexpr int window_width = 1610;
constexpr int window_height = 910;

constexpr int squares_per_line = (window_width - square_gutter) / (square_side + square_gutter);
constexpr int squares_per_column = (window_height - square_gutter) / (square_side + square_gutter);

constexpr int grid_offset_x = window_width - squares_per_line * (square_side + square_gutter);
constexpr int grid_offset_y = window_height - squares_per_column * (square_side + square_gutter);

bool cells[squares_per_line][squares_per_column];

constexpr GLfloat white[] = {1, 1, 1, 1};
constexpr GLfloat black[] = {0, 0, 0, 0};
constexpr GLfloat grey[] = {.5, .5, .5, .5};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  cursor_x = xpos;
  cursor_y = ypos;
}

void find_corresponding_cell(double x, double y, int* cell_row, int* cell_col) {
  // cells can be offset by one pixel due to grid_offset * .5 rounding
  *cell_row = floor((x - grid_offset_x * .5) / (double)(square_side + square_gutter));
  *cell_col = floor((y - grid_offset_y * .5) / (double)(square_side + square_gutter));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    int hovered_row, hovered_col;
    find_corresponding_cell(cursor_x, cursor_y, &hovered_row, &hovered_col);
    if (cells[hovered_row][hovered_col] == 1)
      cells[hovered_row][hovered_col] = 0;
    else
      cells[hovered_row][hovered_col] = 1;
  }
}

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

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  float vertices[] = {
      -(square_side * .5) / (window_width * .5), (square_side * .5) / (window_height * .5),  .0,  // top left
      (square_side * .5) / (window_width * .5),  (square_side * .5) / (window_height * .5),  .0,  // top right
      -(square_side * .5) / (window_width * .5), -(square_side * .5) / (window_height * .5), .0,  // bottom left
      (square_side * .5) / (window_width * .5),  -(square_side * .5) / (window_height * .5), .0,  // bottom right
  };

  unsigned int indices[] = {0, 1, 2, 1, 2, 3};

  for (int row = 0; row < squares_per_line; row++) {
    for (int col = 0; col < squares_per_column; col++) {
      cells[row][col] = 0;
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
        // coloring
        int hovered_row, hovered_col;
        find_corresponding_cell(cursor_x, cursor_y, &hovered_row, &hovered_col);
        if ((row + squares_per_line / 2) == hovered_row && (col + (squares_per_column / 2)) == hovered_col)
          glUniform4fv(is_alive_loc, 1, grey);
        else if (cells[row + squares_per_line / 2][col + (squares_per_column / 2)] == 1)
          glUniform4fv(is_alive_loc, 1, black);
        else
          glUniform4fv(is_alive_loc, 1, white);

        // placing
        const float offset_x = (square_side + square_gutter) * row + (square_side * .5);
        const float offset_y = -(square_side + square_gutter) * col - (square_side * .5);
        glUniform2f(offset_loc, offset_x / (window_width * .5), offset_y / (window_height * .5));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
    glBindVertexArray(0);
  }
}