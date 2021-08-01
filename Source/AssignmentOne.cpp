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




// Cursor coordinates
float lastX = 0, lastY = 0;
float deltaX = 0;       //represents the change in X and Y since last mouse event
float deltaY = 0;
float fov = 45.0f;
bool textureState = true;


//Object - Offset Declaration
float movementOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float movementOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float movementOffsetZ[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float rotationOffsetZ[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float scalingOffset[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

int currObject = 3;     // 0 index mapping


//Window Size
float WindowWidth = 1024.0f;
float WindowHeight = 768.0f;

//Colors
GLfloat redColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat greenColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat blueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat whiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat purpleColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
GLfloat bitchColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };


// Input callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

//calculate normal
glm::vec3 calculateNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);


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


/*TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)), //left - red
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, -0.5f),  glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,0.0f,-1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,-1.0f)),

    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)), // far - blue
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)),





    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)), // bottom - turquoise
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f,0.0f,0.0f)),


    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)), // near - green
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f),glm::vec3(1.0f,0.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f,0.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f,  0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f,0.0f,0.0f)),






    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)), // right - purple
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, 0.5f),  glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f)), // top - yellow
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),

    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)),
    TexturedColoredVertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f,1.0f,0.0f))*/

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
        ""
        "in vec2 vertexUV;"
        "in vec3 Normal;"
        "in vec3 FragPos;"
        ""
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   float ambientStrength = 0.3;"
        "   vec3 ambient = ambientStrength * vec3(lightColor);"
        ""
        "   vec3 norm = normalize(Normal);"
        "   vec3 lightDir = normalize(lightPos - FragPos);"
        "   float diff = max(dot(norm, lightDir), 0.0);"
        "   vec3 diffuse = diff * vec3(lightColor);"
        ""
        "   float specularStrength = 0.9;"
        "   vec3 viewDir = normalize(viewPos - FragPos);"
        "   vec3 reflectDir = reflect(-lightDir, norm);"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
        "   vec3 specular = specularStrength * spec * vec3(lightColor);"
        ""
        "   vec4 textureColor = texture( textureSampler, vertexUV );"
        "   vec3 result = (ambient + diffuse + specular) ;"
        ""
        "   if (stateOfTexture)FragColor = textureColor * vec4(result, 1.0); "
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
    glTexImage2D(GL_TEXTURE_2D, 0, format, width/2, height/2,
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


glm::vec3 baseVectorArray[4];
glm::vec3 focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 eyePosition = glm::vec3(0.0f, 40.0f, 0.0f);


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
    
    // Compile and link shaders here ...
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
    
    // Define and upload geometry to the GPU here ...
    int vaoGridLine = createGridLineVertexArrayObject();
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();
    
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

    
    // Disable Backface culling
    glDisable(GL_CULL_FACE);

   

    float goesUp = 0;
    float goesUpTwo = 0;
    GLenum render = GL_TRIANGLES;

    //enable depth test
    glEnable(GL_DEPTH_TEST);

    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    glm::mat4 scalingMatrix;
    glm::mat4 worldMatrix;
    GLuint worldMatrixLocation;
    GLuint colorLocation;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
       
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
        if (textureState)glBindTexture(GL_TEXTURE_2D, brickTextureID);

        GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



        //Setting up light source
        GLuint LightColor = glGetUniformLocation(texturedShaderProgram, "lightColor");
        glUniform4fv(LightColor, 1, whiteColor);

        GLuint LightPos = glGetUniformLocation(texturedShaderProgram, "lightPos");
        GLfloat lightPosition[3] = { 0.0f, 30.0f, 0.0f };
        glUniform3fv(LightPos, 1, lightPosition);

        GLuint ViewPos = glGetUniformLocation(texturedShaderProgram, "viewPos");
        GLfloat viewPostion[3] = { eyePosition.x, eyePosition.y, eyePosition.z };
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
       
        // + x bar
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 1.0f, 1.0f));
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        worldMatrix = scalingMatrix * rotationMatrix;
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
        glUniform4fv(colorLocation, 1, bitchColor);


        //Draw Cubes
        
        glBindVertexArray(texturedCubeVAO);

        glUseProgram(texturedShaderProgram);
        worldMatrixLocation = glGetUniformLocation(texturedShaderProgram, "worldMatrix");
        colorLocation = glGetUniformLocation(texturedShaderProgram, "objectColor");
        

   
        /*
        * THE LIGHT CUBE SHOULD DRAWN USING THE LIGHTCUBE_SHADER - BUT IT IS NOT WORKING :(
        * SO CURRENTLY DRAWING IT USING THE NORMAL SHADER
        * 
            glUseProgram(LightCube_shaderProgram);
            worldMatrixLocation = glGetUniformLocation(LightCube_shaderProgram, "worldMatrix");
            colorLocation = glGetUniformLocation(LightCube_shaderProgram, "objectColor");
        */

        //Drawing cube around light source
        
        translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]));
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(4, 4, 4));
        worldMatrix = translationMatrix * scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(100, 1, 100));
        

        
        //floor
            glBindTexture(GL_TEXTURE_2D, tilesTextureID);
            translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5, 0));
            worldMatrix = translationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
 
        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //wall 1
        

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

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        //Object-1
        baseVectorArray[0] = { 45.5f, 2.5f, 45.5f };
        baseVectorArray[0] += glm::vec3(movementOffsetX[0], movementOffsetY[0], movementOffsetZ[0]);
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
                
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[0]);
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(-x, y, -z) * scalingOffset[0]);

                worldMatrix = translationMatrix * scalingMatrix;

                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
        }

        
        for (int i = 0; i < 5; i++) {

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[0]);
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(-4.0f, i, 0.0f) * scalingOffset[0]);

            worldMatrix = translationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glUniform4fv(colorLocation, 1, whiteColor);
            glDrawArrays(render, 0, 36);
        }

        

      
        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        //wall 2
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 2 && x == 6) || (y == 3 && x == 4) || (y == 3 && x == 6) || (y == 4 && x == 4) || (y == 4 && x == 6) || (y == 5 && x == 4) || (y == 5 && x == 5) || (y == 5 && x == 6) || (y == 5 && x == 7) || (y == 6 && x == 4) || (y == 6 && x == 7))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x + 40.5f) , (y + 0.5f) , -45.5f) );
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        //Object-2
        baseVectorArray[1] = { 45.5f, 2.5f, -35.5f };
        baseVectorArray[1] += glm::vec3(movementOffsetX[1], movementOffsetY[1], movementOffsetZ[1]);
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

                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[1]);
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[1]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[1]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[1]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(x, y, z) * scalingOffset[1]);

                worldMatrix = translationMatrix * scalingMatrix;
                
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
        }

        translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[1]);
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[1]), glm::vec3(1.0f, 0.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[1]), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(translationMatrix, glm::vec3(3.0f, 3.0f, 2.0f) * scalingOffset[1]);

        worldMatrix = translationMatrix * scalingMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glUniform4fv(colorLocation, 1, whiteColor);
        glDrawArrays(render, 0, 36);




        glBindTexture(GL_TEXTURE_2D, brickTextureID);

        //wall 3
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 2 && x == 5) || (y == 3 && x == 3) || (y == 3 && x == 4) || (y == 3 && x == 5) || (y == 3 && x == 6) || (y == 3 && x == 7) || (y == 4 && x == 4) || (y == 4 && x == 5) || (y == 4 && x == 6) || (y == 5 && x == 5))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - 49.5f), (y + 0.5f), 35.5f));
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        //Object-3
        baseVectorArray[2] = { -45.5f, 2.5f, 45.5f };
        baseVectorArray[2] += glm::vec3(movementOffsetX[2], movementOffsetY[2], movementOffsetZ[2]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[2], scalingOffset[2], scalingOffset[2]));

        int k = 1;
        for (int j = 0; j < 2; j++)
        {
            for (int i = -2; i <= 2; i++) {
                
                translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2]);
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
                translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
                translationMatrix = glm::translate(translationMatrix, glm::vec3(i * k, 0.0f, -i) * scalingOffset[2]);

                
                worldMatrix = translationMatrix * scalingMatrix;
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                glUniform4fv(colorLocation, 1, whiteColor);
                glDrawArrays(render, 0, 36);
            }
            k = -1;
        }

        for (int i = -1; i < 3; i++) {
            
            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2]);
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(0.0f, i, 0.0f) * scalingOffset[2]);

            worldMatrix = translationMatrix * scalingMatrix;
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

            translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[2]);
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[2]), glm::vec3(1.0f, 0.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[2]), glm::vec3(0.0f, 1.0f, 0.0f));
            translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            translationMatrix = glm::translate(translationMatrix, glm::vec3(x3, 1, z3) * scalingOffset[2]);


            worldMatrix = translationMatrix * scalingMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glUniform4fv(colorLocation, 1, whiteColor);
            glDrawArrays(render, 0, 36);
        }


        glBindTexture(GL_TEXTURE_2D, brickTextureID);

        //wall 4
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (!((y == 3 && x == 3) || (y == 3 && x == 5) || (y == 3 && x == 7) || (y == 4 && x == 4) || (y == 4 && x == 6) || (y == 5 && x == 5))) {

                    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 4.5f, y + 0.5f, -10.5f) );
                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, blueColor);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        //Object-4
        baseVectorArray[3] = { -0.5f, 2.5f, -0.5f };
        baseVectorArray[3] += glm::vec3(movementOffsetX[3], movementOffsetY[3], movementOffsetZ[3]);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scalingOffset[3], scalingOffset[3], scalingOffset[3]));

 
        for (int j = 0; j < 3; j++)
        {
            for (int i = -2; i <= 2; i++) {
                if (abs(i % 2 == 0)) {
                    
                    translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3]);
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
                    translationMatrix = glm::translate(translationMatrix, glm::vec3(i, 0.0f, j) * scalingOffset[3]);

                    worldMatrix = translationMatrix * scalingMatrix;
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
                    
                    translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3]);
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
                    translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
                    translationMatrix = glm::translate(translationMatrix, glm::vec3(i, 1.0f, j) * scalingOffset[3]);

                    worldMatrix = translationMatrix * scalingMatrix;
                    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
                    glUniform4fv(colorLocation, 1, whiteColor);
                    glDrawArrays(render, 0, 36);
                }
            }
        }
              
        translationMatrix = glm::translate(glm::mat4(1.0f), baseVectorArray[3]);
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetX[3]), glm::vec3(1.0f, 0.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetY[3]), glm::vec3(0.0f, 1.0f, 0.0f));
        translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationOffsetZ[3]), glm::vec3(0.0f, 0.0f, 1.0f));
        translationMatrix = glm::translate(translationMatrix, glm::vec3(0.0f, 2.0f, 0.0f) * scalingOffset[3]);


        worldMatrix = translationMatrix * scalingMatrix;
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


            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            goesUp -= 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).x, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3((sin(goesUp) * radius) + focalPoint.x, eyePosition.y, (cos(goesUp) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// upad


            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            goesUpTwo += 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius)+focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            goesUpTwo -= 0.04;
            float radius = sqrt(pow((eyePosition - focalPoint).y, 2) + pow((eyePosition - focalPoint).z, 2));
            eyePosition = glm::vec3(eyePosition.x, (cos(goesUpTwo) * radius) + focalPoint.y, (-sin(goesUpTwo) * radius) + focalPoint.z);
            glm::mat4 viewMatrix = glm::lookAt((eyePosition),  // eye
                focalPoint,  // center
                glm::vec3(0.0f, 1.0f, 0.0f));// up


            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
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

            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        // tilt the camera in y axis
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            glm::mat4 viewMatrix = glm::lookAt(
                (eyePosition),  // eye
                focalPoint += glm::vec3(0.0f, deltaY, 0.0f),    //center
                glm::vec3(0.0f, 1.0f, 0.0f));// up

            GLuint viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        }

        //continous translation along z axis
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)       
            movementOffsetZ[currObject] += 1;

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            movementOffsetZ[currObject] -= 1;

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
    {
        currObject = 0;
        focalPoint = baseVectorArray[currObject];
        eyePosition = focalPoint + glm::vec3(0.0f, 0.0f, 20.0f);

    }

    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        currObject = 1;
        focalPoint = baseVectorArray[currObject];
        eyePosition = focalPoint + glm::vec3(0.0f, 0.0f, 20.0f);
    }

    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        currObject = 2;
        focalPoint = baseVectorArray[currObject];
        eyePosition = focalPoint + glm::vec3(0.0f, 0.0f, 20.0f);
    }

    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        currObject = 3;
        focalPoint = baseVectorArray[currObject];
        eyePosition = focalPoint + glm::vec3(0.0f, 0.0f, 20.0f);
    }
    //switch texturing modes
    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        textureState = !textureState;
    }



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