#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <objload.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Generate terrain vertices and indices from heightmap
void generateTerrain(const unsigned char* heightData, int width, int height, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float y = heightData[z * width + x] / 255.0f; // Normalize height to 0.0 - 1.0
            vertices.push_back(x);          // X-coordinate
            vertices.push_back(y * 10.0f);  // Y-coordinate (scaled height)
            vertices.push_back(z);          // Z-coordinate
            vertices.push_back(x / float(width)); // Texture X
            vertices.push_back(z / float(height)); // Texture Y
        }
    }

    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}



int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Heightmap Generator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // Load heightmap image
    int imgWidth, imgHeight, imgChannels;
    unsigned char* heightData = stbi_load("D:/Miv2/sim/project/tp3/assets/Small Ridge Through Terrain/heightmap.png", &imgWidth, &imgHeight, &imgChannels, 1);
    if (!heightData) {
        std::cerr << "Failed to load heightmap image!" << std::endl;
        return -1;
    }

    // Generate terrain data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateTerrain(heightData, imgWidth, imgHeight, vertices, indices);
    stbi_image_free(heightData);

    // Create VAO and VBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Create shaders
    GLuint ShaderProgram = LoadShaders("D:/Miv2/sim/project/tp3/shader/SimpleVertexShader.vertexshader",
        "D:/Miv2/sim/project/tp3/shader/SimpleFragmentShader.fragmentshader");

    // MVP matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP = Projection * View * Model;

    GLuint MatrixID = glGetUniformLocation(ShaderProgram, "MVP");

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ShaderProgram);

        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(ShaderProgram);

    glfwTerminate();
    return 0;
}