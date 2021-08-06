
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
        
uniform mat4 worldMatrix;
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);
        
out vec3 FragPos;
out vec3 Normal;
out vec2 vertexUV;
                
void main()
{
    FragPos = vec3(worldMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(worldMatrix))) * aNormal;
    mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
    gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
vertexUV = aUV;
}