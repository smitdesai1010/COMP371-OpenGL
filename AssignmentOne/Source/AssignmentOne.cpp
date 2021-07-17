//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices


// Global variables
float lastX = 0, lastY = 0;
float deltaX = 0;       //represents the change in X and Y since last mouse event
float deltaY = 0;
float fov = 45.0f;


// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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
//vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
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

int createVertexArrayObject()
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

int createCubeVertexArrayObject(/*float x, float y, float z, float r, float g, float b*/)
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

    // Setting mouse cursor as input mode
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
    int vao = createVertexArrayObject();
    int vaoCube = createCubeVertexArrayObject();

    
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glm::vec3 eyePosition = glm::vec3(0.0f, 40.0f, 0.0f);

    //speed of movement initialisation
    float speed;
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);
        
        GLfloat redColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
        GLfloat greenColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
        GLfloat blueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        GLfloat whiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat purpleColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };

        // Draw geometry
        glUseProgram(shaderProgram);
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        GLuint colorLocation = glGetUniformLocation(shaderProgram, "Color");

        glBindVertexArray(vao);
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

        glUniform4fv(colorLocation, 1, greenColor);

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
        

        
        //Draw Cube
        glUseProgram(shaderProgram);
        glBindVertexArray(vaoCube);
        /*scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        worldMatrix = scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);*/

        glm::vec3 baseVector = { 0.0f, 0.0f, 0.0f};
        float x = 48.0f, z=46.0f;
        baseVector = baseVector + glm::vec3(x, 0.0f, z);

        //Draw Object
        for (int k = -3; k <= 3; k++)
        {
            for (int i = -2; i <= 2; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    glUniform4fv(colorLocation, 1, whiteColor);
                    translationMatrix = glm::translate(glm::mat4(1.0f), baseVector + glm::vec3(i, j, k));
                    worldMatrix = translationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                
                if (k % 2 == 0) {
                    glUniform4fv(colorLocation, 1, purpleColor);
                }
                else {
                    glUniform4fv(colorLocation, 1, blueColor);
                }
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVector + glm::vec3(i, 2.0f, k));
                worldMatrix = translationMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        for (int i = -2; i <= 2; i++)
        {
            if (i % 2 == 0) {
                glUniform4fv(colorLocation, 1, redColor);
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVector + glm::vec3(i, 3.0f, 0.0f));
                worldMatrix = translationMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        

        


        


        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //inputs
 
        // close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

       //sprint movement and default speed
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed = 0.5f;
        }
        else {
            speed = 0.25f;
        }

        // View Transform
      
        // right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            eyePosition += glm::vec3(speed, 0.0f, 0.0f);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
               // eyePosition + glm::vec3(0.0f, -0.5f, -1.0f),  // center
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        // left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // shift camera to the left
        {
                eyePosition += glm::vec3(-speed, 0.0f, 0.0f);
                glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                //    eyePosition + glm::vec3(0.0f, -0.5f, -1.0f),  // center
                    glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));// up
                

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        // forwards
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            eyePosition += glm::vec3(0.0f, 0.0f, -speed);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
               // eyePosition + glm::vec3(0.0f, -0.5f, -1.0f),  // center
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        // backwards
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // shift camera to the left
        {
            eyePosition += glm::vec3(0.0f, 0.0f, speed);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye

               // eyePosition + glm::vec3(0.0f, -0.5f, -1.0f),  // center
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));// upad


            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        // perspective Transform
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),  // field of view in degrees
            1024.0f / 768.0f,      // aspect ratio
            0.01f, 1000.0f);       // near and far (near > 0)

        GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
   
                


        //need to change to right mouse btn
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // pan the camera in x axis
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
            eyePosition += glm::vec3(0.0f, deltaY, 0.0f);
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                glm::vec3(0.0f, 0.0f, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

       
        //need to change to left mouse btn
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) // zoom in zoom out
        {
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1024.0f / 768.0f, 0.1f, 1000.0f);
            
            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }
    }
    
    // Shutdown GLFWhh
    glfwTerminate();
    
    return 0;
}




void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    deltaX = xpos - lastX;
    deltaY = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;

    if (fov < 1.0f)
        fov = 1.0f;
}

