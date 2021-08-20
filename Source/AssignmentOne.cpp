//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>

#include <cmath>

#include<math.h>


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#include <irrKlang.h>   // Sound library
#pragma comment(lib, "irrKlang.lib")

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <set>
#include <list>
#include <algorithm>
#include <../VS2017/Shader.h>
#include <vector>
#include <../VS2017/Camera.h>
#include "../VS2017/OBJloader.h"


// Cursor coordinates
float lastX = 0, lastY = 0;
float deltaX = 0;       //represents the change in X and Y since last mouse event
float deltaY = 0;
float fov = 45.0f;
bool textureState = true;


//Object - Offset Declaration
glm::vec3 movementOffset = glm::vec3{0,0,0};
float rotationOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetZ[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float scalingOffset[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

int currObject = 2;     // 0 index mapping


//
int object3Vertices;
GLuint object3VAO;


//Window Size
float WindowWidth = 1024.0f;
float WindowHeight = 768.0f;

//Colors
glm::vec4 redColor = { 1.0f, 0.0f, 0.0f, 1.0f };
glm::vec4 greenColor = { 0.0f, 1.0f, 0.0f, 1.0f };
glm::vec4 blueColor = { 0.0f, 0.0f, 1.0f, 1.0f };
glm::vec4 whiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };
glm::vec4 purpleColor = { 1.0f, 0.0f, 1.0f, 1.0f };
glm::vec4 bitchColor = { 1.0f, 1.0f, 1.0f, 0.0f };


//uniform variables
glm::vec3 baseVector;
glm::vec3 focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 eyePosition = glm::vec3(0.0f, 40.0f, 0.0f);

glm::mat4 translationMatrix;
glm::mat4 rotationMatrix;
glm::mat4 scalingMatrix;
glm::mat4 worldMatrix;
bool nextModel = true;

glm::vec3 lightPosition = { 0.0f,30.0f, 0.0f };
bool shadows = false;
bool shadowsKeyPressed = false;

//render mode, default = triangles
GLenum render = GL_TRIANGLES;


//Sounds
irrklang::ISoundEngine* soundEngine;
void collisionSound();
void successSound();

//Coordinates for 0-9 numbers for text rendering
//It is rendered using cubes, much like we did in QUIZ-2 by a 4*3 rectangle, (0,0) corresponds to bottom left
std::set<std::pair<int, int>> setXY[10] = {
   { {0,4}, {1,4}, {2,4}, {2,3}, {2,2}, {0,2}, {0,1}, {0,0}, {1,0}, {2,0}, {0,3}, {2,1} },        //0
   { {1,4}, {1,3}, {1,2}, {1,1}, {1,0} },                                                         //1
   { {0,4}, {1,4}, {2,4}, {2,3}, {2,2}, {1,2}, {0,2}, {0,1}, {0,0}, {1,0}, {2,0} },               //2
   { {0,4}, {1,4}, {2,4}, {2,3}, {2,2}, {1,2}, {0,2}, {2,1}, {2,0}, {1,0}, {0,0} },               //3
   { {0,4}, {0,3}, {0,2}, {1,2}, {2,2}, {2,3}, {2,4}, {2,1}, {2,0} },                             //4
   { {0,4}, {1,4}, {2,4}, {0,3}, {0,2}, {1,2}, {2,2}, {2,1}, {2,0}, {1,0}, {0,0} },               //5
   { {0,4}, {1,4}, {2,4}, {0,3}, {0,2}, {1,2}, {2,2}, {2,1}, {2,0}, {1,0}, {0,0}, {0,1} },        //6
   { {0,4}, {1,4}, {2,4}, {2,3}, {2,2}, {2,1}, {2,0} },                                           //7
   { {0,4}, {1,4}, {2,4}, {2,3}, {2,2}, {1,2}, {0,2}, {0,1}, {0,0}, {1,0}, {2,0}, {0,3}, {2,1} }, //8
   { {0,4}, {1,4}, {2,4}, {0,3}, {0,2}, {1,2}, {2,2}, {2,1}, {2,0}, {1,0}, {0,0}, {2,3} }         //9
};

int score = 0;
const int TOTALTIME = 180; //3 mins
void renderDigit(Shader shader, int X, int Y, int digit);
void renderScore(Shader shader);
void renderTime(Shader shader);


// Input callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

//calculate normal
glm::vec3 calculateNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);


//replaces all vertices in cube with colored vertex for texture mapping

GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width/2, height/2,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

unsigned int gridLineVAO = 0;
unsigned int gridLineVBO = 0;


//renderCube(render) renders a unit cube
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f,  0.0f,  1.0f, -1.0f, 0.0f, 0.0f, // top-right
            -0.5f,  0.5f,  0.5f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, // bottom-right         
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // top-right
            -0.5f,  0.5f,  0.5f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, // top-left
            // front face
             0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f, 0.0f, -1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 0.0f, -1.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, -1.0f, // top-right
             0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f, 0.0f, -1.0f, // top-right
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, -1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 0.0f, -1.0f, // bottom-left
            // left face
             0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  0.0f, -1.0f, 0.0f, // top-right
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f, 0.0f, // top-left
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, -1.0f, 0.0f, // bottom-left
             0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f, 0.0f, // top-right
            // right face
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // top-right         
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left     
            // bottom face
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // top-right
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // top-left
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // bottom-right
             0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, // top-right
            // top face
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-left
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right     
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(render, 0, 36);
    glBindVertexArray(0);
}

void nextRandModel();

//renderGridLine() renders a line
void renderGridLine()
{
    if (gridLineVAO == 0)
    {
        glm::vec3 vertices[] = {
        glm::vec3(0.0f,  0.0f, 0.0f),  // one side
        glm::vec3(1.0f,  0.0f, 0.0f),  // top center color (red)
        glm::vec3(100.0f, 0.0f, 0.0f),  // the other
        glm::vec3(0.0f,  1.0f, 0.0f),  // bottom right color (green)
        };

        glGenVertexArrays(1, &gridLineVAO);
        glGenBuffers(1, &gridLineVBO);

        //fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, gridLineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //link vertex attributes
        glBindVertexArray(gridLineVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    //render gridLine
    glBindVertexArray(gridLineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

GLuint setupModelVBO(std::string path, int& vertexCount) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    // read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);  // Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
    // attribute pointer(s).

    // Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
        &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(1);

    // UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent
    // strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}

void renderScene(const Shader &shader, const GLuint brick, const GLuint cement, const GLuint tiles)
{
    //rendering score
    renderScore(shader);
    //rendring Time;
    renderTime(shader);

    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec4("objectColor", greenColor);
    /*
    for (int i = 0; i < 101; i++) {
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 0.0f, -50.0f + i));
        worldMatrix = translationMatrix * scalingMatrix;
        shader.setMat4("worldMatrix", worldMatrix);
        shader.setVec4("objectColor", greenColor);
        renderGridLine();
    }

    rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    for (int i = 0; i < 101; i++) {
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f + i, 0.0f, 50.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotationMatrix;

        shader.setMat4("worldMatrix", worldMatrix);
        shader.setVec4("objectColor", greenColor);
        renderGridLine();
    }*/

    // + x bar
    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 1.0f, 1.0f));
    rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    worldMatrix = scalingMatrix * rotationMatrix;
    shader.setMat4("worldMatrix", worldMatrix);
    shader.setVec4("objectColor", whiteColor);
    renderGridLine();

    // + z bar
    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.05f));
    rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    worldMatrix = scalingMatrix * rotationMatrix;
    shader.setMat4("worldMatrix", worldMatrix);
    shader.setVec4("objectColor", redColor);
    renderGridLine();

    // + y bar
    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.05f, 1.0f));
    rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    worldMatrix = scalingMatrix * rotationMatrix;
    shader.setMat4("worldMatrix", worldMatrix);
    shader.setVec4("objectColor", blueColor);
    renderGridLine();
    shader.setVec4("objectColor", bitchColor);


    


    //floor
    if (textureState)
    {
        glBindTexture(GL_TEXTURE_2D, tiles);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(100, 1, 100));
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, 0));
        worldMatrix = translationMatrix * scalingMatrix;
        shader.setMat4("worldMatrix", worldMatrix);
        renderCube();
    }




    //Draw Cubes

    //Drawing cube around light source
    shader.setBool("lighting", false);
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]));
    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
    worldMatrix = translationMatrix * scalingMatrix;
    shader.setMat4("worldMatrix", worldMatrix);
    shader.setVec4("objectColor", whiteColor);
    renderCube();
    shader.setBool("lighting", true);



    glBindTexture(GL_TEXTURE_2D, brick);
    scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
    int z3 = 0, x3 = 0 , k = 0;
    if (nextModel) {
        baseVector = glm::vec3{ 0,3,10 };
        nextModel = false;
    }
    
    int wallOffset = -5;
    switch(currObject){
    case 0:
    {
        //wall 1
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 11; x++) {
                if (!((y == 3 && x == 3) || (y == 3 && x == 4) || (y == 3 && x == 5) || (y == 3 && x == 6) || (y == 3 && x == 7) || (y == 4 && x == 3) || (y == 4 && x == 7) || (y == 5 && x == 3) || (y == 5 && x == 7) || (y == 6 && x == 3) || (y == 6 && x == 4) || (y == 6 && x == 5) || (y == 6 && x == 6) || (y == 6 && x == 7) || (y == 7 && x == 3))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x+ wallOffset, y ,0));
                    worldMatrix = translationMatrix * scalingMatrix;
                    shader.setMat4("worldMatrix", worldMatrix);
                    shader.setVec4("objectColor", blueColor);
                    renderCube();
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, cement);
        baseVector += movementOffset;
        
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[0], scalingOffset[0], scalingOffset[0]));

        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++)
            {
                int x = 0, y = 0, z = 0;

                if (j == 0) {
                    x = i;
                }
                else if (j == 1) {
                    y = i;
                }
                else if (j == 2) {
                    z = i;
                }
                else if (j == 3) {
                    y = 3;
                    x = i;
                    z = i;
                }

                translationMatrix = glm::translate(glm::mat4(1.0f), baseVector+glm::vec3(2,0,0));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(-x, y, -z) * scalingOffset[0]);

                worldMatrix = translationMatrix * scalingMatrix;
                shader.setMat4("worldMatrix", worldMatrix);
                shader.setVec4("objectColor", whiteColor);
                renderCube();
            }
        }


        for (int i = 0; i < 5; i++) {

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVector + glm::vec3(2, 0, 0));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(-4.0f, i, 0.0f) * scalingOffset[0]);

            worldMatrix = translationMatrix * scalingMatrix;
            shader.setMat4("worldMatrix", worldMatrix);
            shader.setVec4("objectColor", whiteColor);
            renderCube();
        }


    }
        break;
    case 1:
    {
        glBindTexture(GL_TEXTURE_2D, brick);
        //wall 2
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 11; x++) {
                if (!((y == 2 && x == 6) || (y == 3 && x == 4) || (y == 3 && x == 6) || (y == 4 && x == 4) || (y == 4 && x == 6) || (y == 5 && x == 4) || (y == 5 && x == 5) || (y == 5 && x == 6) || (y == 5 && x == 7) || (y == 6 && x == 4) || (y == 6 && x == 7))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + wallOffset, y , 0));
                    worldMatrix = translationMatrix * scalingMatrix;
                    shader.setMat4("worldMatrix", worldMatrix);
                    shader.setVec4("objectColor", blueColor);
                    renderCube();
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, cement);
        //Object-2
        baseVector += movementOffset;
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[1], scalingOffset[1], scalingOffset[1]));

        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {

                int x = 0, y = 0, z = 0;

                if (j == 0) {
                    z = i;
                }
                else if (j == 1) {
                    y = i;
                    z = 2;
                }
                else if (j == 2) {
                    x = i;
                    y = 2;
                    z = 2;
                }
                else if (j == 3) {
                    x = 2;
                    y = 2 - i;
                    z = 2;
                }
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVector - glm::vec3(1, 0, 0));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[1]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[1]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[1]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(x, y, z) * scalingOffset[1]);

                worldMatrix = translationMatrix * scalingMatrix;
                shader.setMat4("worldMatrix", worldMatrix);
                shader.setVec4("objectColor", whiteColor);
                renderCube();
            }
        }

        translationMatrix = glm::translate(glm::mat4(1.0f), baseVector - glm::vec3(1, 0, 0));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[1]), glm::vec3(1.0f, 0.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[1]), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(translationMatrix, glm::vec3(3.0f, 3.0f, 2.0f) * scalingOffset[1]);

        worldMatrix = translationMatrix * scalingMatrix;
        shader.setMat4("worldMatrix", worldMatrix);
        shader.setVec4("objectColor", whiteColor);
        renderCube();
    }

        break;
    case 2:
    {
        glBindTexture(GL_TEXTURE_2D, brick);
        //wall 3
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 11; x++) {
                if (!((y == 2 && x == 5) || (y == 3 && x == 3) || (y == 3 && x == 4) || (y == 3 && x == 5) || (y == 3 && x == 6) || (y == 3 && x == 7) || (y == 3 && x == 8) || (y == 4 && x == 4) || (y == 4 && x == 5) || (y == 4 && x == 6) || (y==5 && x==5))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x + wallOffset), (y ), 0));
                    worldMatrix = translationMatrix * scalingMatrix;
                    shader.setMat4("worldMatrix", worldMatrix);
                    shader.setVec4("objectColor", blueColor);
                    renderCube();
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, cement);
        //Object-3
      
        baseVector += movementOffset;
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[2], scalingOffset[2], scalingOffset[2]));

        int k = 1;
        for (int j = 0; j < 2; j++)
        {
            for (int i = -2; i <= 2; i++) {

                translationMatrix = glm::translate(glm::mat4(1.0f), baseVector);
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(i * k, 0.0f, -i) * scalingOffset[2]);


                worldMatrix = translationMatrix * scalingMatrix;
                shader.setMat4("worldMatrix", worldMatrix);
                shader.setVec4("objectColor", whiteColor);
                renderCube();
            }
            k = -1;
        }

        for (int i = -1; i < 3; i++) {

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVector);
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(0.0f, i, 0.0f) * scalingOffset[2]);

            worldMatrix = translationMatrix * scalingMatrix;
            shader.setMat4("worldMatrix", worldMatrix);
            shader.setVec4("objectColor", whiteColor);
            renderCube();
        }

        int x3 = 1, z3 = 1;
        for (int i = 0; i < 3; i++) {

            if (i > 0 && i % 2 == 1)
            {
                x3 *= -1;
            }
            else if (i > 0 && i % 2 == 0) {
                z3 *= -1;
            }

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVector);
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(x3, 1, z3) * scalingOffset[2]);


            worldMatrix = translationMatrix * scalingMatrix;
            shader.setMat4("worldMatrix", worldMatrix);
            shader.setVec4("objectColor", whiteColor);
            renderCube();
        }
        translationMatrix = glm::translate(glm::mat4(1.0f), baseVector);
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(translationMatrix, glm::vec3(3, 0, 2) * scalingOffset[2]);


        worldMatrix = translationMatrix * scalingMatrix;
        shader.setMat4("worldMatrix", worldMatrix);
        shader.setVec4("objectColor", whiteColor);
        renderCube();
    }
        break;
    case 3:
    {
        glBindTexture(GL_TEXTURE_2D, brick);

        //wall 4
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 11; x++) {
                if (!((y == 3 && x == 3) || (y == 3 && x == 5) || (y == 3 && x == 7) || (y == 4 && x == 4) || (y == 4 && x == 6))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + wallOffset, y , 0));
                    worldMatrix = translationMatrix * scalingMatrix;
                    shader.setMat4("worldMatrix", worldMatrix);
                    shader.setVec4("objectColor", blueColor);
                    renderCube();
                }
            }
        }
        
        //Object-4
       
        baseVector += movementOffset;
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[3], scalingOffset[3], scalingOffset[3]));

        glBindVertexArray(object3VAO);
        glBindTexture(GL_TEXTURE_2D, cement);
        translationMatrix = glm::translate(glm::mat4(1.0f), baseVector);
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(translationMatrix, glm::vec3(0.0f, 0.0f, 0.0f) * scalingOffset[3]);

        worldMatrix = translationMatrix * scalingMatrix;
        shader.setMat4("worldMatrix", worldMatrix);
        shader.setVec4("objectColor", whiteColor);
        glDrawArrays(GL_QUADS, 0, object3Vertices);
                    
    }
        break;
    }
    movementOffset = glm::vec3(0, 0, 0);

   


    /*
    translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3]);
    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
    translationMatrix = glm::translate(translationMatrix, glm::vec3(0.0f, 2.0f, 0.0f) * scalingOffset[3]);


    worldMatrix = translationMatrix * scalingMatrix;
    shader.setMat4("worldMatrix", worldMatrix);
    shader.setVec4("objectColor", whiteColor);
    renderCube();*/




    glBindVertexArray(0);
}



int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    // Sound Engine
    soundEngine = irrklang::createIrrKlangDevice();
    
#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif


    // Define and upload geometry to the GPU here ...
    /*int vaoGridLine = createGridLineVertexArrayObject();
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();*/

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    WindowWidth = mode->width;
    WindowHeight = mode->height;

    // Create Window and rendering context using GLFW, resolution is set to the current screen size
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Assignment 1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Setting input callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //
    object3Vertices;
    object3VAO = setupModelVBO("../Assets/Models/object3.obj", object3Vertices);

    
    // Compile and link shaders here ...
    Shader sceneShader("vertexShader.vs", "fragmentShader.fs");
    Shader simpleDepthShader("point_shadows_depth.vs", "point_shadows_depth.fs", "point_shadows_depth.gs");
    
    
    // Load Textures
#if defined(PLATFORM_OSX)
    GLuint brickTextureID = loadTexture("Textures/brick.jpg");
    GLuint cementTextureID = loadTexture("Textures/cement.jpg");
    GLuint tilesTextureID = loadTexture("Textures/tiles.jpg");
#else
    GLuint brickTextureID = loadTexture("../Assets/Textures/brick.jpg");
    GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
    GLuint tilesTextureID = loadTexture("../Assets/Textures/tiles.jpg");
#endif

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    // --------------------
    sceneShader.use();
    sceneShader.setInt("diffuseTexture", 0);
    sceneShader.setInt("depthMap", 1);


    
    // Disable Backface culling
    glDisable(GL_CULL_FACE);


    float goesUp = 0;
    float goesUpTwo = 0;

    //enable depth test
    glEnable(GL_DEPTH_TEST);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // playing sound
    soundEngine->play2D("../Assets/Audio/background.mp3", true);

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {

        if (baseVector.z < 0) {
            std::string d = std::to_string(rotationOffsetX[currObject]) +"," + std::to_string(rotationOffsetY[currObject]) +","+ std::to_string(rotationOffsetZ[currObject]);
            printf("%s\n", d.c_str());
            if (((((int)rotationOffsetX[currObject] % 360) == 0)  && (((int)rotationOffsetY[currObject] % 360) == 0)  && (((int)rotationOffsetZ[currObject] % 360) == 0))
                || ((((int)rotationOffsetX[currObject] % 360) == 180) && (((int)rotationOffsetY[currObject] % 360) == 180) && (((int)rotationOffsetZ[currObject] % 360) == 180))) {
                score += 20;
            }
            else if (score>0) {
                score -= 5;
            }
            nextRandModel();
        }

        // Each frame, reset color of each pixel to glClearColor and depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneShader.setBool("stateOfTexture", textureState);


        shadowsKeyPressed = false;
        // perspective Transform
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(fov),     // field of view in degrees
            WindowWidth / WindowHeight,      // aspect ratio
            0.01f, 
            100000.0f              // near and far (near > 0)
        );    


        sceneShader.setMat4("projectionMatrix", projectionMatrix);
       
      


        //update look at function as soon as we switch object
        glm::mat4 viewMatrix = glm::lookAt(
            (eyePosition),      // eye
            focalPoint,         // center
            glm::vec3(0.0f, 1.0f, 0.0f) // up
        );

        //sceneShader.setMat4("viewMatrix", viewMatrix);

        if (textureState)glBindTexture(GL_TEXTURE_2D, brickTextureID);

        sceneShader.setMat4("viewMatrix", viewMatrix);


        //Setting up light source

        sceneShader.setVec4("lightColor", whiteColor);

        sceneShader.setVec3("lightPos", lightPosition);


        glm::vec3 viewPosition = { eyePosition.x, eyePosition.y, eyePosition.z };
        sceneShader.setVec3("viewPos", viewPosition);


        
        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPosition);
        renderScene(simpleDepthShader, brickTextureID, cementTextureID, tilesTextureID);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        glViewport(0, 0, WindowWidth, WindowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sceneShader.use();
        //glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
        //glm::mat4 view = camera.GetViewMatrix();
        //sceneShader.setMat4("projectionMatrix", projection);
        //sceneShader.setMat4("viewMatrix", view);
        // set lighting uniforms
        //sceneShader.setVec3("lightPos", lightPosition);
        //sceneShader.setVec3("viewPos", camera.Position);
        sceneShader.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
        sceneShader.setFloat("far_plane", far_plane);/*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tilesTextureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);*/
        renderScene(sceneShader, brickTextureID, cementTextureID, tilesTextureID);


        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();


        // close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetTime() > 180)   //close window after 180s
            glfwSetWindowShouldClose(window, true);


        //Change render mode

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) 
        {
            render = GL_POINTS;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) 
        {
            render = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            render = GL_TRIANGLES;
        }

   


        // WORLD-CAMERA INTERACTION

        // backwards
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            goesUp += 0.04;
            
            float radius = sqrt(pow((eyePosition - focalPoint).x, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3((sin(goesUp) * radius) + focalPoint.x  ,eyePosition.y, (cos(goesUp) * radius)+focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            sceneShader.setMat4("viewMatrix", viewMatrix);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            goesUp -= 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).x, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3((sin(goesUp) * radius) + focalPoint.x, eyePosition.y, (cos(goesUp) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// upad


            sceneShader.setMat4("viewMatrix", viewMatrix);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            goesUpTwo += 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius)+focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            sceneShader.setMat4("viewMatrix", viewMatrix);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            goesUpTwo -= 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            sceneShader.setMat4("viewMatrix", viewMatrix);
        }

        //reset the world orientation to origin
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
        {
            eyePosition = { 0.0f,40.0f,0.0f };
            focalPoint = { 0.0f,0.0f,0.0f };
        }


        // pan the camera in x axis
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) 
        {
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                focalPoint += glm::vec3(deltaX, 0.0f, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));   // up

            sceneShader.setMat4("viewMatrix", viewMatrix);
        }

        // tilt the camera in y axis
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                focalPoint += glm::vec3(0.0f, deltaY, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            sceneShader.setMat4("viewMatrix", viewMatrix);
        }

        //continous translation along z axis
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)       
            movementOffset += glm::vec3(0,0,1);

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            movementOffset -= glm::vec3(0,0,1);

        baseVector -= glm::vec3{ 0,0,0.01 };
        focalPoint = baseVector;
    }

   
  
    // Shutdown GLFWhh
    glfwTerminate();
    
    return 0;
}

void nextRandModel() {
    
        int object = (rand() % 4);

        currObject = object;

        int randInt = (rand() % 4) * 90;
        rotationOffsetX[object] += randInt;

        randInt = (rand() % 4) * 90;
        rotationOffsetY[object] += randInt;

        randInt = (rand() % 4) * 90;
        rotationOffsetZ[object] += randInt;

        baseVector = { 0.0f, 3.0f, 10.0f };

}

//The callback will make the object move one unit at one press AND RELEASE
//it WON'T move continuously till the key is press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //switch texturing modes
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        textureState = !textureState;
    }



    //scaling
    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
        scalingOffset[currObject] += 0.25;

    else if (key == GLFW_KEY_M && action == GLFW_PRESS)
        scalingOffset[currObject] = scalingOffset[currObject] > 0.25 ? scalingOffset[currObject] - 0.25 : 0.25;


    //translation
    /*
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        movementOffset += glm::vec3(0, 1, 0);

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        movementOffset -= glm::vec3(0, 1, 0);

    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        movementOffset += glm::vec3(1, 0, 0);
    
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        movementOffset -= glm::vec3(1, 0, 0);
   
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        movementOffset += glm::vec3(0, 0, 1);

    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
        movementOffset -= glm::vec3(0, 0, 1);
    */
    //rotation
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) //x-axis
        rotationOffsetX[currObject] += 90;

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        rotationOffsetX[currObject] -= 90;

    else if (key == GLFW_KEY_E && action == GLFW_PRESS) //y-axis
        rotationOffsetY[currObject] += 90;

    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        rotationOffsetY[currObject] -= 90;

    else if (key == GLFW_KEY_D && action == GLFW_PRESS) //z-axis
        rotationOffsetZ[currObject] += 90;

    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        rotationOffsetZ[currObject] -= 90;


 

    else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    deltaX = xpos - lastX;
    deltaY = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.3f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) // zoom in zoom out
    {
        fov -= (float)yoffset;

        if (fov < 1.0f)
            fov = 1.0f;
    }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
}


glm::vec3 calculateNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 dir = glm::cross(b - a, c - b);
    glm::vec3 norm = glm::normalize(dir);

    return norm;
}


void renderScore(Shader shader)
{
    shader.setBool("stateOfTexture", false);

    if (score > 9)  //double digits
    {
        renderDigit(shader, 19, 5, (score / 10) % 10);
        renderDigit(shader, 22, 5, score % 10);
    }

    else
        renderDigit(shader, 19, 5, score);
    
    shader.setBool("stateOfTexture", textureState);
}


void renderTime(Shader shader)
{
    //the window closes after 180s
    int timeRemaining = TOTALTIME - glfwGetTime();

    if (timeRemaining / 100 > 0)      //3 digits
    {
        int digitOne = timeRemaining % 10;  //one's place
        int digitTwo = (timeRemaining / 10) % 10;   //ten's place
        int digitThree = (timeRemaining / 100) % 10;    //hundred's place

  
        renderDigit(shader,-25,5, digitThree);
        renderDigit(shader, -22, 5, digitTwo);
        renderDigit(shader, -19, 5, digitOne);
    }


    if (timeRemaining / 10 > 0)      //2 digits
    {
        int digitOne = timeRemaining % 10;  //one's place
        int digitTwo = (timeRemaining / 10) % 10;   //ten's place

        renderDigit(shader, -22, 5, digitTwo);
        renderDigit(shader, -19, 5, digitOne);
    }


    else
        renderDigit(shader, -19, 5, timeRemaining);
    

}


void renderDigit(Shader shader, int X, int Y, int digit)
{
    if (digit > 9)
    {
        std::cout << "DIGITS GREATER THAN 9 NOT ALLOWED\n";
        return;
    }

    shader.setBool("stateOfTexture", false);
    glm::vec3 scalingOffsetDigit = glm::vec3(0.5, 0.5, 0.5);

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 5; ++j)
        {
            if (setXY[digit].find({ i,j }) == setXY[digit].end())
                continue;

            worldMatrix = glm::translate(glm::mat4(1), glm::vec3(X, Y, 0));
            worldMatrix = glm::translate(worldMatrix, glm::vec3(i,j,0) * scalingOffsetDigit);
            worldMatrix = glm::scale(worldMatrix, scalingOffsetDigit);

            shader.setMat4("worldMatrix", worldMatrix);
            shader.setVec4("objectColor", redColor);
            renderCube();
        }


    shader.setBool("stateOfTexture", textureState);
}


void collisionSound()
{
    //soundEngine->stopAllSounds();
    irrklang::createIrrKlangDevice()->play2D("../Assets/Audio/collision.wav", false);
    //soundEngine->play2D("../Assets/Audio/collision.wav", true);
}

void successSound()
{
    irrklang::createIrrKlangDevice()->play2D("../Assets/Audio/sucess.wav", false);
}