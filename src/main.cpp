#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

GLFWwindow* window = nullptr;

uint32_t wndWidth = 1280;
uint32_t wndHeight = 720;

int main(int argc, char** argv)
{
    assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(wndWidth, wndHeight, "Compute Shader", nullptr, nullptr);
    assert(window);
    glfwMakeContextCurrent(window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    glViewport(0, 0, wndWidth, wndHeight);


    glm::vec3 lightPos{ 0.5f, 1.0f, 0.3f };

    Shader shader("shader/normal_mapping.vert", "shader/normal_mapping.frag");

    Camera camera({ 2.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f });

    Texture color("res/brickwall.jpg");
    Texture normalMap("res/brickwall_normal.jpg");


    uint32_t plane = 0;
    uint32_t vbo;
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;
            glm::vec3 tangent;
            glm::vec3 bitangent;
        };

        // positions
        glm::vec3 pos1(-1.0, 1.0, 0.0);
        glm::vec3 pos2(-1.0, -1.0, 0.0);
        glm::vec3 pos3(1.0, -1.0, 0.0);
        glm::vec3 pos4(1.0, 1.0, 0.0);
        // texture coordinates
        glm::vec2 uv1(0.0, 1.0);
        glm::vec2 uv2(0.0, 0.0);
        glm::vec2 uv3(1.0, 0.0);
        glm::vec2 uv4(1.0, 1.0);
        // normal vector
        glm::vec3 nm(0.0, 0.0, 1.0);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // - triangle 1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // - triangle 2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        Vertex vertices[] =
        {
            { pos1, nm, uv1, tangent1, tangent1 },
            { pos2, nm, uv2, tangent1, tangent1 },
            { pos3, nm, uv3, tangent1, tangent1 },
            { pos1, nm, uv1, tangent2, tangent2 },
            { pos3, nm, uv3, tangent2, tangent2 },
            { pos4, nm, uv4, tangent2, tangent2 },
        };


        glGenVertexArrays(1, &plane);
        glBindVertexArray(plane);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    }


    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double newTime = glfwGetTime();
        double dt = (newTime - lastTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        

        {
            shader.bind();

            shader.setValue("proj", camera.getProj());
            shader.setValue("view", camera.getView());
            shader.setValue("viewPos", camera.pos);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, color.texture);
            shader.setValue("diffuseMap", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap.texture);
            shader.setValue("normalMap", 1);

            shader.setValue("lightPos", lightPos);

            glm::mat4 model;
            model = glm::rotate(glm::mat4(1.0f), (float)glm::radians(glfwGetTime() * 30.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
            model = glm::rotate(model, (float)glm::radians(glfwGetTime() * 50.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            shader.setValue("model", model);

            glBindVertexArray(plane);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
}