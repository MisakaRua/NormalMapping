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

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

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

    window = glfwCreateWindow(wndWidth, wndHeight, "Normal Mapping", nullptr, nullptr);
    assert(window);
    glfwMakeContextCurrent(window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    glViewport(0, 0, wndWidth, wndHeight);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");


    glm::vec3 lightPos{ 0.5f, 1.0f, 0.3f };

    Shader shader("shader/normal_mapping.vert", "shader/normal_mapping.frag");

    Camera camera({ 3.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });

    Texture color("res/brickwall.jpg");
    Texture normalMap("res/brickwall_normal.jpg");
    Texture displancementlMap("res/bricks_disp.jpg");


    uint32_t box = 0;
    uint32_t vbo = 0;
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;
            glm::vec3 tangent;
            glm::vec3 bitangent;
        };


        Vertex vertices[] =
        {
            { glm::vec3{ 1.0f, -1.0f, -1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },

            { glm::vec3{-1.0f,  1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },

            { glm::vec3{-1.0f, -1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f, -1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },

            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f,  1.0f,  1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },

            { glm::vec3{-1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f,  1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },

            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },

            { glm::vec3{ 1.0f,  1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },

            { glm::vec3{ 1.0f, -1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },

            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f, -1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f, -1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },

            { glm::vec3{ 1.0f, -1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f, -1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },

            { glm::vec3{ 1.0f,  1.0f, -1.0f}, {}, glm::vec2{1.0f, 1.0f}, {}, {} },
            { glm::vec3{-1.0f,  1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },

            { glm::vec3{-1.0f,  1.0f,  1.0f}, {}, glm::vec2{0.0f, 0.0f}, {}, {} },
            { glm::vec3{ 1.0f,  1.0f,  1.0f}, {}, glm::vec2{1.0f, 0.0f}, {}, {} },
            { glm::vec3{-1.0f,  1.0f, -1.0f}, {}, glm::vec2{0.0f, 1.0f}, {}, {} }
        };

        for (int i = 0; i < sizeof(vertices) / (3 * sizeof(Vertex)); ++i)
        {
            glm::vec3 edge1 = vertices[i * 3 + 1].position - vertices[i * 3].position;
            glm::vec3 edge2 = vertices[i * 3 + 2].position - vertices[i * 3].position;

            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            vertices[i * 3].normal = normal;
            vertices[i * 3 + 1].normal = normal;
            vertices[i * 3 + 2].normal = normal;

            glm::vec2 deltaUV1 = vertices[i * 3 + 1].texCoord - vertices[i * 3].texCoord;
            glm::vec2 deltaUV2 = vertices[i * 3 + 2].texCoord - vertices[i * 3].texCoord;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);
            vertices[i * 3].tangent = tangent;
            vertices[i * 3 + 1].tangent = tangent;
            vertices[i * 3 + 2].tangent = tangent;

            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);
            vertices[i * 3].bitangent = bitangent;
            vertices[i * 3 + 1].bitangent = bitangent;
            vertices[i * 3 + 2].bitangent = bitangent;
        }


        glGenVertexArrays(1, &box);
        glBindVertexArray(box);

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


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    int isNormalMapping = 1;
    float heightScale = 0.1f;

    while (!glfwWindowShouldClose(window))
    {
        float timeRadians = glm::radians(glfwGetTime());

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();

        shader.setValue("proj", camera.getProj());
        shader.setValue("view", camera.getView());
        shader.setValue("viewPos", camera.pos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, color.texture);
        shader.setValue("DiffuseMap", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap.texture);
        shader.setValue("NormalMap", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, displancementlMap.texture);
        shader.setValue("DepthMap", 2);

        lightPos = { timeRadians * 17.0f, timeRadians * 21.0f, timeRadians * 37.0f };
        shader.setValue("lightPos", lightPos);

        glm::mat4 model;
        model = glm::rotate(glm::mat4(1.0f), timeRadians * 30.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
        model = glm::rotate(model, (float)glm::radians(glfwGetTime() * 50.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
        shader.setValue("model", model);

        shader.setValue("isNormalMapping", isNormalMapping);
        shader.setValue("height_scale", heightScale);

        glBindVertexArray(box);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            ImGui::Begin("Mapping Algorithm");
            if (ImGui::Button("Switch Mapping Algorithm"))
            {
                isNormalMapping = 1 - isNormalMapping;
                ImGui::Text(isNormalMapping ? "Current: Normal Mapping" : "Current: Parallax Mapping");
            }

            ImGui::SliderFloat("Height Scale", &heightScale, 0.001f, 0.2f);
            ImGui::End();


            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &box);


    glfwDestroyWindow(window);
    glfwTerminate();
}