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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <list>
#include <algorithm>

float fov = 45.0f;
bool textureState = true;

//Window Size
float WindowWidth = 1024.0f;
float WindowHeight = 768.0f;

//Colors
GLfloat redColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat greenColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat whiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat purpleColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };


// Input callback
void window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//replaces all vertices in cube with colored vertex for texture mapping
struct TexturedColoredVertex
{
    TexturedColoredVertex(glm::vec3 _position, glm::vec2 _uv, glm::vec3 _normal)
        : position(_position), uv(_uv), normal(_normal) {}

    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};


// Textured Cube model
const TexturedColoredVertex texturedCubeVertexArray[] = {  // position,                            
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f),glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)), //left - red
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)), // far - blue
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)), // bottom - turquoise
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)), // near - green
    TexturedColoredVertex(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)), // right - purple
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f)), // top - yellow
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f))
};


int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
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

const char* getTexturedVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec2 aUV;"
        "layout (location = 2) in vec3 aNormal;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 EyeDir;"
        "out vec3 FragPos;"
        "out vec3 Normal;"
        "out vec2 vertexUV;"
        ""
        "void main()"
        "{"
        "   FragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        "   Normal = mat3(transpose(inverse(worldMatrix))) * aNormal;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "   vertexUV = aUV;"
        "}";
}
const char* getTexturedFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "#define LINE_WIDTH 2.5 \n"
        "uniform vec3 lightPos;"
        "uniform vec3 viewPos;"
        "uniform vec4 lightColor;"
        "uniform vec4 objectColor;"
        "uniform sampler2D textureSampler;"
        "uniform bool stateOfTexture;"
        "uniform int shininess;"
        "uniform bool ambientOn;"
        ""
        "in vec2 vertexUV;"
        "in vec3 Normal;"
        "in vec3 FragPos;"
        ""
        "out vec4 FragColor;"
        "void main()"
        "{"
        "float ambientStrength;"
        "if (ambientOn) ambientStrength = 0.2;"
        "else ambientStrength = 0;"
        "   vec3 ambient = ambientStrength * vec3(lightColor);"
        ""
        "   vec3 norm = normalize(Normal);"
        "   vec3 lightDir = normalize(lightPos - FragPos);"
        "   float diff = max(dot(norm, lightDir), 0.0);"
        "   vec3 diffuse = diff * vec3(lightColor);"
        ""
        "   float specularStrength = 1;"
        "   vec3 viewDir = normalize(viewPos - FragPos);"
        "   vec3 reflectDir = reflect(-lightDir, norm);"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);"
        "   vec3 specular = specularStrength * spec * vec3(lightColor);"
        ""
        "   vec4 textureColor = texture( textureSampler, vertexUV );"
        "   vec3 result = (ambient + diffuse + specular) ;"
        "   if (stateOfTexture)FragColor = textureColor * vec4(result  , 1.0);"
        "   else FragColor =  vec4(result * vec3(objectColor), 1.0);"
        "}";
}


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
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
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
int createTexturedCubeVertexArrayObject()
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(TexturedColoredVertex), // stride 
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(
        1,                            // attribute 1 matches aUV in Vertex Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)(sizeof(glm::vec3))      // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(1);
    


    glVertexAttribPointer(
        2,                            // attribute 2 matches aNormal in Vertex Shader
        3,
        GL_FLOAT,
        GL_TRUE,
        sizeof(TexturedColoredVertex),
        (void*)(sizeof(glm::vec3)+sizeof(glm::vec2))     
    );
    glEnableVertexAttribArray(2);

    return vertexArrayObject;
}



void setWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}
void setViewMatrix(int shaderProgram, glm::mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

glm::vec3 focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 eyePosition = glm::vec3(0.0f, 40.0f, 0.0f);
glm::vec3 positionOffset = glm::vec3(-12, 0, -2);

int corner = 0;
bool riding = false;
bool ambientOn = true;

int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    float t = 0;
    
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
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);


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
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
    int lightCubeShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());

    // Define and upload geometry to the GPU here ...
    int vaoGridLine = createGridLineVertexArrayObject();
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();
    
    // Load Textures
#if defined(PLATFORM_OSX)
    GLuint brickTextureID = loadTexture("Textures/brick.jpg");
    GLuint cementTextureID = loadTexture("Textures/cement.jpg");
    GLuint tilesTextureID = loadTexture("Textures/tiles.jpg");
#else
    
    GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
    GLuint tilesTextureID = loadTexture("../Assets/Textures/tiles.jpg");
    GLuint brickTextureID = loadTexture("../Assets/Textures/brick.jpg");
    GLuint brickTwoTextureID = loadTexture("../Assets/Textures/brick2.jpg");
    GLuint brickThreeTextureID = loadTexture("../Assets/Textures/brick3.jpg");
    GLuint brickFourTextureID = loadTexture("../Assets/Textures/brick4.jpg");
    GLuint wheelsTextureID = loadTexture("../Assets/Textures/wheels.jpg");
    GLuint skateBoardTextureID = loadTexture("../Assets/Textures/board.jpg");
    GLuint redTextureID = loadTexture("../Assets/Textures/red.jpg");
    GLuint blueTextureID = loadTexture("../Assets/Textures/blue.jpg");
    GLuint greenTextureID = loadTexture("../Assets/Textures/green.jpg");
#endif

    
    // Disable Backface culling
    glDisable(GL_CULL_FACE);

    GLenum render = GL_TRIANGLES;

    //enable depth test
    glEnable(GL_DEPTH_TEST);

    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    glm::mat4 skateBoardRotationMatrix;
    glm::mat4 scalingMatrix;
    glm::mat4 worldMatrix;
    GLuint worldMatrixLocation;
    GLuint colorLocation;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
       
        GLuint shhhh = glGetUniformLocation(texturedShaderProgram, "shininess");
        glUniform1i(shhhh, 128);
        
        GLuint ambi = glGetUniformLocation(texturedShaderProgram, "ambientOn");
        glUniform1i(ambi, ambientOn);

        // Each frame, reset color of each pixel to glClearColor and depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint StateOfTexture = glGetUniformLocation(texturedShaderProgram, "stateOfTexture");
        glUniform1i(StateOfTexture, textureState);

        // perspective Transform
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(fov),     // field of view in degrees
            WindowWidth / WindowHeight,      // aspect ratio
            0.01f, 
            100000.0f              // near and far (near > 0)
        );    

        GLuint projectionMatrixLocation = glGetUniformLocation(texturedShaderProgram, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
       
      


        //update look at function as soon as we switch object
        glm::mat4 viewMatrix = glm::lookAt(
            (eyePosition),      // eye
            focalPoint,         // center
            glm::vec3(0.0f, 1.0f, 0.0f) // up
        );

        setViewMatrix(texturedShaderProgram, viewMatrix);
       

        GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



        //Setting up light source
        GLfloat lightPosition[3] = { 0.0f, 50.0f, 0.0f };
        GLfloat viewPostion[3] = { eyePosition.x, eyePosition.y, eyePosition.z };
        
        GLuint LightColor = glGetUniformLocation(texturedShaderProgram, "lightColor");
        glUniform4fv(LightColor, 1, whiteColor);

        GLuint LightPos = glGetUniformLocation(texturedShaderProgram, "lightPos");
        glUniform3fv(LightPos, 1, lightPosition);

        GLuint ViewPos = glGetUniformLocation(texturedShaderProgram, "viewPos");
        glUniform3fv(ViewPos, 1, viewPostion);

        


        // Draw Grid
        glUseProgram(texturedShaderProgram);
        worldMatrixLocation = glGetUniformLocation(texturedShaderProgram, "worldMatrix");
        colorLocation = glGetUniformLocation(texturedShaderProgram, "objectColor");

        glBindVertexArray(vaoGridLine);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniform4fv(colorLocation, 1, greenColor);

        for (int i = 0; i < 101; i++) {
             translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 0.0f, -50.0f + i));
             worldMatrix = translationMatrix * scalingMatrix;

             glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
             glDrawArrays(GL_LINES, 0, 2);
        }

        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        for (int i = 0; i < 101; i++) {
            translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f + i, 0.0f, 50.0f));
            worldMatrix = translationMatrix * scalingMatrix * rotationMatrix;

            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            
            glDrawArrays(GL_LINES, 0, 2);
        }
        glLineWidth(25);
      
        // + x bar
        glBindTexture(GL_TEXTURE_2D, redTextureID);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 1.0f, 1.0f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(GL_LINES, 0, 2);
        
        // + z bar
        glBindTexture(GL_TEXTURE_2D, blueTextureID);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.05f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, redColor);
        glDrawArrays(GL_LINES, 0, 2);
      
        // + y bar
        glBindTexture(GL_TEXTURE_2D, greenTextureID);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.05f, 1.0f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_LINES, 0, 2);
        glLineWidth(1);

        glBindVertexArray(0);
      

        //Draw Cubes
        
        glBindVertexArray(texturedCubeVAO);

        glUseProgram(texturedShaderProgram);
        worldMatrixLocation = glGetUniformLocation(texturedShaderProgram, "worldMatrix");
        colorLocation = glGetUniformLocation(texturedShaderProgram, "objectColor");

        //skycube
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1000, 1000, 1000));
        worldMatrix = translationMatrix * scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        
        
        glUniform1i(shhhh, 32);
        float boardAngle = 0;
        skateBoardRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(boardAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        //skate board
        glBindTexture(GL_TEXTURE_2D, skateBoardTextureID);
        for (int z = 0; z <= 5; z++) {
            for (int x = 0; x <= 25; x++) {
                
                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x , 2, z)+positionOffset);
                    worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);

            }
        }
        for (int z = 0; z <= 5; z++) {
            for (int x = -3; x < 1; x++) {

                translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 3, z) + positionOffset);
                worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, blueColor);
                glDrawArrays(GL_TRIANGLES, 0, 36);

            }
        }
        for (int z = 0; z <= 5; z++) {
            for (int x = 25; x < 29; x++) {

                translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 3, z) + positionOffset);
                worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, blueColor);
                glDrawArrays(GL_TRIANGLES, 0, 36);

            }
        }
        glBindTexture(GL_TEXTURE_2D, wheelsTextureID);
        //wheels
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 0) + positionOffset);
        worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 5) + positionOffset);
        worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(24, 1, 5) + positionOffset);
        worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(24, 1, 0) + positionOffset);
        worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, blueColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //numbers
        //4
        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 25; x++) {
                if (((y == 1 && x == 5) || (y == 2 && x == 5) || (y == 3 && x == 5) || (y == 4 && x == 5) || (y == 4 && x == 4) || (y == 4 && x == 3) || (y == 4 && x == 2) || (y == 4 && x == 1) || (y == 5 && x == 1) || (y == 5 && x == 5) || (y == 6 && x == 5) || (y == 6 && x == 1) || (y == 7 && x == 5) || (y == 7 && x == 1))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x , y+2 , 2) + positionOffset);
                    worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //numbers
        //0
        glBindTexture(GL_TEXTURE_2D, brickTwoTextureID);
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 25; x++) {
                if (((y == 1 && x == 7) || (y == 1 && x == 8) || (y == 1 && x == 9) || (y == 1 && x ==10) || (y == 1 && x ==11) || (y == 2 && x == 7) || (y == 2 && x ==11) || (y == 3 && x == 7) || (y == 3 && x ==11) || (y == 4 && x == 7) || (y == 4 && x ==11) || (y == 5 && x == 7) || (y == 5 && x ==11) || (y == 6 && x == 7) || (y == 6 && x ==11) || (y == 7 && x == 7) || (y == 7 && x == 8) || (y == 7 && x == 9) || (y == 7 && x == 10) || (y == 7 && x == 11) || (y == 4 && x == 9))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 2, 2) + positionOffset);
                    worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //numbers
        //9
        glBindTexture(GL_TEXTURE_2D, brickThreeTextureID);
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 25; x++) {
                if (((y == 1 && x == 14) || (y == 1 && x == 15) || (y == 1 && x == 16) || (y == 1 && x == 17) || (y == 2 && x == 17) || (y == 3 && x == 17) || (y == 4 && x == 13) || (y == 4 && x == 14) || (y == 4 && x == 15) || (y == 4 && x == 16) || (y == 4 && x == 17) || (y == 5 && x == 13) || (y == 5 && x == 17) || (y == 6 && x == 13) || (y == 6 && x == 17) || (y == 7 && x == 13) || (y == 7 && x == 14) || (y == 7 && x == 15) || (y == 7 && x == 16) || (y == 7 && x == 17))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 2, 2) + positionOffset);
                    worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //numbers
       //6
        glBindTexture(GL_TEXTURE_2D, brickFourTextureID);
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 25; x++) {
                if (((y == 1 && x == 19) || (y == 1 && x == 20) || (y == 1 && x == 21) || (y == 1 && x == 22) || (y == 1 && x == 23) || (y == 2 && x == 19) || (y == 2 && x == 23) || (y == 3 && x == 19) || (y == 3 && x == 23) || (y == 4 && x == 19) || (y == 4 && x == 20) || (y == 4 && x == 21) || (y == 4 && x == 22) || (y == 4 && x == 23) || (y == 5 && x == 19) || (y == 6 && x == 19) || (y == 7 && x == 19) || (y == 7 && x == 20) || (y == 7 && x == 21) || (y == 7 && x == 22) || (y == 7 && x == 23))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 2, 2) + positionOffset);
                    worldMatrix = translationMatrix * scalingMatrix * skateBoardRotationMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        



        glBindVertexArray(0);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
 
        // close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //moving the skateboard
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            positionOffset += glm::vec3(1, 0, 0);
            if (riding) {
                eyePosition = glm::vec3(12, 3, 5) + positionOffset;
                focalPoint = glm::vec3(0, 0, -5) + eyePosition;
            }
            
        }
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            positionOffset -= glm::vec3(1, 0, 0);
            if (riding) {
                eyePosition = glm::vec3(12, 3, 5) + positionOffset;
                focalPoint = glm::vec3(0, 0, -5) + eyePosition;
            }
        }
        
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {

            float a = 20;
            float b = 10;
            t+=0.01;
            positionOffset = glm::vec3(a * cos(t),0, b * sin(t));
            
            if (riding) {
                eyePosition = glm::vec3(12, 3, 5) + positionOffset;
                focalPoint = glm::vec3(0, 0, -5) + eyePosition;
            }
        }

    }

    // Shutdown GLFWhh
    glfwTerminate();
    
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //camera selection
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        riding = false;
        eyePosition = glm::vec3(0,40,40);
        focalPoint = glm::vec3(0, 0, 0);
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        riding = true;
        eyePosition = glm::vec3(12, 3, 5) + positionOffset;
        focalPoint = eyePosition + glm::vec3(0, 0, -5);
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        riding = false;
        corner++;
        corner = corner % 4;
        
        switch (corner)
        {
        case 0:
            eyePosition = glm::vec3(50, 50, 50);
            break;
        case 1:
            eyePosition = glm::vec3(50, 50, -50);
            break;
        case 2:
            eyePosition = glm::vec3(-50, 50, 50);
            break;
        case 3:
            eyePosition = glm::vec3(-50, 50, -50);
            break;
        }
        focalPoint =  glm::vec3(0, 0, 0);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        ambientOn = !ambientOn;
    }

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
}

