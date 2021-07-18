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


// Cursor coordinates
float lastX = 0, lastY = 0;
float deltaX = 0;       //represents the change in X and Y since last mouse event
float deltaY = 0;
float fov = 45.0f;

//speed of camera movement initialisation
float speed;

//Object - Offset Declaration
float movementOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float movementOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float movementOffsetZ[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetZ[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

float scalingOffset[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
int currObject = 0;     // 0 index mapping



//Colors
GLfloat redColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat greenColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat whiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat purpleColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };


// Input callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);"
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}
const char* getFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "#define LINE_WIDTH 2.5 \n"
                "uniform vec4 Color; \n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   FragColor = Color; "
                "}";

    
}
int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}
int createGridLineVertexArrayObject()
{
    glm::vec3 vertexArrayLine[] = {
    glm::vec3(0.0f,  0.0f, 0.0f),  // one side
    glm::vec3(1.0f,  0.0f, 0.0f),  // top center color (red)
    glm::vec3(100.0f, 0.0f, 0.0f),  // the other
    glm::vec3(0.0f,  1.0f, 0.0f),  // bottom right color (green)
    };
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArrayLine), vertexArrayLine, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          2*sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          2*sizeof(glm::vec3),
                          (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return vertexArrayObject;
}
int createCubeVertexArrayObject()
{

    GLfloat vertexArray[] = {
    -0.5f,-0.5f,-0.5f, // triangle 1 : begin
    -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, // triangle 1 : end
    0.5f, 0.5f,-0.5f, // triangle 2 : begin
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f, // triangle 2 : end
    0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    0.5f,-0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f,-0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f,-0.5f, 0.5f
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);



    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        0, // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}




int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
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

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Assignment 1", NULL, NULL);
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


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    
    // Define and upload geometry to the GPU here ...
    int vaoGridLine = createGridLineVertexArrayObject();
    int vaoCube = createCubeVertexArrayObject();

    
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glm::vec3 eyePosition = glm::vec3(0.0f, 40.0f, 0.0f);
    glm::vec3 focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 baseVectorArray[4];

//<<<<<<< HEAD
    //speed of movement initialisation
    float speed;
    float goesUp = 0;
    float goesUpTwo = 0;
    GLenum render = GL_TRIANGLES;
//=======
//>>>>>>> main
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);
        
        // perspective Transform
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),  // field of view in degrees
            1024.0f / 768.0f,      // aspect ratio
            0.01f, 100000.0f);       // near and far (near > 0)

        GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);



        // Draw Grid
        glUseProgram(shaderProgram);
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        GLuint colorLocation = glGetUniformLocation(shaderProgram, "Color");

        glBindVertexArray(vaoGridLine);
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform4fv(colorLocation, 1, greenColor);

        glm::mat4 translationMatrix;
        for (int i = 0; i < 101; i++) {
             translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 0.0f, -50.0f + i));
             glm::mat4 worldMatrix = translationMatrix * scalingMatrix;

             glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
             glDrawArrays(GL_LINES, 0, 2);
        }
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (int i = 0; i < 101; i++) {
            translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f + i, 0.0f, 50.0f));
            glm::mat4 worldMatrix = translationMatrix * scalingMatrix * rotationMatrix;

            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            
            glDrawArrays(GL_LINES, 0, 2);
        }
       
        // + x bar
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 1.0f, 1.0f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        glm::mat4 worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(GL_LINES, 0, 2);
        
        // + z bar
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.05f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, redColor);
        glDrawArrays(GL_LINES, 0, 2);
      
        // + y bar
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.05f, 1.0f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_LINES, 0, 2);


        glBindVertexArray(0);
        

        
        //Draw Objects
        glUseProgram(shaderProgram);
        glBindVertexArray(vaoCube);
        //wall 1
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 3 && x == 3) || (y == 3 && x == 4) || (y == 3 && x == 5) || (y == 3 && x == 6) || (y == 3 && x == 7) || (y == 4 && x == 3) || (y == 4 && x == 7) || (y == 5 && x == 3) || (y == 5 && x == 7) || (y == 6 && x == 3) || (y == 6 && x == 4) || (y == 6 && x == 5) || (y == 6 && x == 6) || (y == 6 && x == 7) || (y == 7 && x == 3))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 40.5f, y + 0.5f, 35.5f));
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //Object-1
        baseVectorArray[0] = { 45.5f, 2.5f, 45.5f };
        baseVectorArray[0] += glm::vec3(movementOffsetX[0], movementOffsetY[0], movementOffsetZ[0]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[0], scalingOffset[0], scalingOffset[0]));

        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));

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

                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[0] + glm::vec3(0.0f - x, 0.0f + y, 0.0f - z));
                worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
        }

        for (int i = 0; i < 5; i++) {
            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[0] + glm::vec3(-4.0f, 0.0f + i, 0.0f));
            worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glUniform4fv(colorLocation, 1, whiteColor);
            glDrawArrays(render, 0, 36);
        }
        //wall 2
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 2 && x == 6) || (y == 3 && x == 4) || (y == 3 && x == 6) || (y == 4 && x == 4) || (y == 4 && x == 6) || (y == 5 && x == 4) || (y == 5 && x == 5) || (y == 5 && x == 6) || (y == 5 && x == 7) || (y == 6 && x == 4) || (y == 6 && x == 7))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 40.5f, y + 0.5f, -45.5f));
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //Object-2
        baseVectorArray[1] = { 45.5f, 2.5f, -35.5f };
        baseVectorArray[1] += glm::vec3(movementOffsetX[1], movementOffsetY[1], movementOffsetZ[1]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[1], scalingOffset[1], scalingOffset[1]));
       
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetX[1]), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
        
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

                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[1] + glm::vec3(0.0f + x, 0.0f + y, 0.0f + z));
                worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
        }

        translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[1] + glm::vec3(3.0f, 3.0f, 2.0f));
        worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(render, 0, 36);
        //wall 3
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 2 && x == 5) || (y == 3 && x == 3) || (y == 3 && x == 4) || (y == 3 && x == 5) || (y == 3 && x == 6) || (y == 3 && x == 7) || (y == 4 && x == 4) || (y == 4 && x == 5) || (y == 4 && x == 6) || (y == 5 && x == 5))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 49.5f, y + 0.5f, 35.5f));
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //Object-3
        baseVectorArray[2] = { -45.5f, 2.5f, 45.5f };
        baseVectorArray[2] += glm::vec3(movementOffsetX[2], movementOffsetY[2], movementOffsetZ[2]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[2], scalingOffset[2], scalingOffset[2]));

        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));

        int k = 1;
        for (int j = 0; j < 2; j++)
        {
            for (int i = -2; i <= 2; i++) {
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2] + glm::vec3(0.0f + i*k, 0.0f, 0.0f - i));
                worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
            k = -1;
        }

        for (int i = -1; i < 3; i++) {
            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2] + glm::vec3(0.0f, 0.0f + i, 0.0f));
            worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glUniform4fv(colorLocation, 1, whiteColor);
            glDrawArrays(render, 0, 36);
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

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2] + glm::vec3(0.0f + x3, 0.0f + 1, 0.0f + z3));
            worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glUniform4fv(colorLocation, 1, whiteColor);
            glDrawArrays(render, 0, 36);
        }

        //wall 4
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 3 && x == 3) || (y == 3 && x == 5) || (y == 3 && x == 7) || (y == 4 && x == 4) || (y == 4 && x == 6) || (y == 5 && x == 5))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 4.5f, y + 0.5f, -10.5f));
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        //Object-4
        baseVectorArray[3] = { -0.5f, 2.5f, -0.5f };
        baseVectorArray[3] += glm::vec3(movementOffsetX[3], movementOffsetY[3], movementOffsetZ[3]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[3], scalingOffset[3], scalingOffset[3]));

        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix += glm::rotate(glm::mat4(1.0f), glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
        
        for (int j = 0; j < 3; j++)
        {
            for (int i = -2; i <= 2; i++) {
                if (abs(i % 2 == 0)) {
                    translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3] + glm::vec3(0.0f + i, 0.0f, 0.0f + j));
                    worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, whiteColor);
                    glDrawArrays(render, 0, 36);
                }
            }
        }
        
        for (int j = 0; j < 2; j++)
        {
            for (int i = -2; i <= 2; i++) {
                if (abs(i % 2) == 1) {
                    translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3] + glm::vec3(0.0f + i, 1.0f, 0.0f + j));
                    worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, whiteColor);
                    glDrawArrays(render, 0, 36);
                }
            }
        }
      
        translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3] + glm::vec3(0.0f, 2.0f, 0.0f));
        worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(render, 0, 36);
        
   


        glBindVertexArray(0);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
 
        // close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // WORLD-CAMERA INTERACTION
        
        //sprint movement and default speed
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 0.8f;
        }
        else {
            speed = 0.4f;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // pan the camera in x axis
        {
            render = GL_POINTS;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // pan the camera in x axis
        {
            render = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // pan the camera in x axis
        {
            render = GL_TRIANGLES;
        }

  
           // backwards
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            goesUp += 0.01;
            
            float radius = sqrt(pow((eyePosition - focalPoint).x, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3((sin(goesUp) * radius) + focalPoint.x  ,eyePosition.y, (cos(goesUp) * radius)+focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            goesUp -= 0.01;
            float radius = sqrt(pow((eyePosition - focalPoint).x, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3((sin(goesUp) * radius) + focalPoint.x, eyePosition.y, (cos(goesUp) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// upad


            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            goesUpTwo += 0.01;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius)+focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            goesUpTwo -= 0.01;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        //need to change to right mouse btn
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // pan the camera in x axis
        {
            eyePosition += glm::vec3(deltaX, 0.0f, 0.0f);
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                glm::vec3(0.0f, 0.0f, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));   // up

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        //need to change to middle mouse btn
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) // tilt the camera in y axis
        {
            //eyePosition += glm::vec3(0.0f, deltaY, 0.0f);
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                focalPoint += glm::vec3(0.0f, deltaY, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        //continous translation along z axis
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)       
            movementOffsetZ[currObject] += 1;

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            movementOffsetZ[currObject] -= 1;

        focalPoint = baseVectorArray[currObject];
    }

    
  
    // Shutdown GLFWhh
    glfwTerminate();
    
    return 0;
}


//The callback will make the object move one unit at one press AND RELEASE
//it WON'T move continuously till the key is press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //object selection
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        currObject = 0;

    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        currObject = 1;

    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        currObject = 2;

    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        currObject = 3;



    //scaling
    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
        scalingOffset[currObject] += 0.25;

    else if (key == GLFW_KEY_M && action == GLFW_PRESS)
        scalingOffset[currObject] = scalingOffset[currObject] > 0.25 ? scalingOffset[currObject] - 0.25 : 0.25;


    //translation
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        movementOffsetY[currObject] += 1;

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        movementOffsetY[currObject] -= 1;

    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        movementOffsetX[currObject] += 1;

    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        movementOffsetX[currObject] -= 1;

    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        movementOffsetZ[currObject] += 1;

    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
        movementOffsetZ[currObject] -= 1;


    //rotation
    else if (key == GLFW_KEY_H && action == GLFW_PRESS) //x-axis
        rotationOffsetX[currObject] += 20;

    else if (key == GLFW_KEY_J && action == GLFW_PRESS)
        rotationOffsetX[currObject] -= 20;

    else if (key == GLFW_KEY_F && action == GLFW_PRESS) //y-axis
        rotationOffsetY[currObject] += 20;

    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
        rotationOffsetY[currObject] -= 20;

    else if (key == GLFW_KEY_V && action == GLFW_PRESS) //z-axis
        rotationOffsetZ[currObject] += 20;

    else if (key == GLFW_KEY_B && action == GLFW_PRESS)
        rotationOffsetZ[currObject] -= 20;


    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        movementOffsetX[currObject] = 0;
        movementOffsetY[currObject] = 0;
        movementOffsetZ[currObject] = 0;
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
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) // zoom in zoom out
    {
        fov -= (float)yoffset;

        if (fov < 1.0f)
            fov = 1.0f;
    }
}

