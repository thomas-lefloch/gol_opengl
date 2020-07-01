#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>

#include <helpers/RootDir.h>

#include "shader.hpp"

double cursor_x = 0;
double cursor_y = 0;

// double to avoid type casting on division
constexpr double update_fps = 9;

constexpr int square_side = 10;
constexpr int square_gutter = 1;

constexpr int window_width = 1610;
constexpr int window_height = 910;

constexpr int squares_per_line = (window_width - square_gutter) / (square_side + square_gutter);
constexpr int squares_per_column = (window_height - square_gutter) / (square_side + square_gutter);

constexpr int grid_offset_x = window_width - squares_per_line * (square_side + square_gutter);
constexpr int grid_offset_y = window_height - squares_per_column * (square_side + square_gutter);

int cells[squares_per_line][squares_per_column];

bool should_update = false;

constexpr GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
constexpr GLfloat grey[] = {.5f, .5f, .5f, 0.8f};

// TODO: support window resizing
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

// TODO: support mouse hold
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

void space_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) should_update = !should_update;
}

void update_cells(int cells[squares_per_line][squares_per_column]) {
  int new_cells[squares_per_line][squares_per_column];
  std::copy(&cells[0][0], &cells[0][0] + squares_per_line * squares_per_column, &new_cells[0][0]);

  for (int row = 0; row < squares_per_line; row++) {
    for (int col = 0; col < squares_per_column; col++) {
      int neighbors = 0;
      // TODO: refactor neighbors counting
      if (row > 0 && col > 0) {
        neighbors += cells[row - 1][col - 1];
        neighbors += cells[row - 1][col];
        neighbors += cells[row][col - 1];
        if (row < squares_per_line - 1 && col < squares_per_column - 1) {
          neighbors += cells[row + 1][col + 1];
          neighbors += cells[row + 1][col];
          neighbors += cells[row][col + 1];
          neighbors += cells[row + 1][col - 1];
          neighbors += cells[row - 1][col + 1];
        }
      } else if (row > 0) {  // col = 0
        neighbors += cells[row - 1][col + 1];
        neighbors += cells[row - 1][col];
        neighbors += cells[row][col + 1];
        if (row < squares_per_line - 1) {
          neighbors += cells[row + 1][col + 1];
          neighbors += cells[row + 1][col];
        }
      } else if (col > 0) {  // row = 0
        neighbors += cells[row][col - 1];
        neighbors += cells[row + 1][col - 1];
        neighbors += cells[row + 1][col];
        if (col < squares_per_column - 1) {
          neighbors += cells[row][col + 1];
          neighbors += cells[row + 1][col + 1];
        }
      } else {  // row = 0 && col = 0
        neighbors += cells[row + 1][col];
        neighbors += cells[row][col + 1];
        neighbors += cells[row + 1][col + 1];
      }

      if (neighbors == 3) {
        new_cells[row][col] = 1;
      } else if (neighbors != 2) {
        new_cells[row][col] = 0;
      }
    }
  }
  std::copy(&new_cells[0][0], &new_cells[0][0] + squares_per_line * squares_per_column, &cells[0][0]);
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
  glfwSetKeyCallback(window, space_callback);

  float vertices[] = {
      -(square_side * .5f) / (window_width * .5f), (square_side * .5f) / (window_height * .5f),  .0f,  // top left
      (square_side * .5f) / (window_width * .5f),  (square_side * .5f) / (window_height * .5f),  .0f,  // top right
      -(square_side * .5f) / (window_width * .5f), -(square_side * .5f) / (window_height * .5f), .0f,  // bottom left
      (square_side * .5f) / (window_width * .5f),  -(square_side * .5f) / (window_height * .5f), .0f,  // bottom right
  };

  unsigned int indices[] = {0, 1, 2, 1, 2, 3};

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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  int shader_id = create_shader_program(ROOT_DIR "shaders/vertex.vs", ROOT_DIR "shaders/fragment.fs");
  if (shader_id == -1) std::cout << "Error while parsing/compiling shaders" << std::endl;

  double total_time = 0;
  while (!glfwWindowShouldClose(window)) {
    double start_time = glfwGetTime();
    glfwSwapBuffers(window);
    glfwPollEvents();

    // TODO: should display indication that game is stopped
    if ((1 / update_fps) - total_time < 0.001 && should_update) {
      update_cells(cells);
      total_time = 0;
    }

    // render
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

    total_time += glfwGetTime() - start_time;
  }
}