#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <objload.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



GLuint IBO;

using namespace glm;
struct STRVertex
{
    vec3 position;
    vec3 couleur;
};


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Set up OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL TP 3", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    std::vector<vec3> position;
    std::vector<vec2> texture;
    std::vector<vec3> normals;

  




    //loadOBJ("D:/Miv2/sim/project/tp3/modeel/torus.obj", position, texture, normals);
    loadOBJ("D:/Miv2/sim/project/tp3/modeel/terrain.obj", position, texture, normals);


    GLuint VAO, VBO;
    //generate vao and vbo
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(position.size()) * sizeof(vec3), &position[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    GLuint VBOColor;
    glGenBuffers(1, &VBOColor);
    glBindBuffer(GL_ARRAY_BUFFER, VBOColor);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint ShaderProgram = LoadShaders("D:/Miv2/sim/project/tp3/shader/SimpleVertexShader.vertexshader",
        "D:/Miv2/sim/project/tp3/shader/SimpleFragmentShader.fragmentshader");


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    mat4 View = lookAt(vec3(0, 2, 2), vec3(0, 1, 1), vec3(0, 1, 0));
    mat4 Model = mat4(1.0f);
    mat4 MVP = Projection * View * Model;

    GLuint MatrixID = glGetUniformLocation(ShaderProgram, "MVP");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while (!glfwWindowShouldClose(window)) {

       // glUseProgram(ShaderProgram);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, position.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }






    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
