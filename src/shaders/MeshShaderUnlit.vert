#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 2) in vec2 uvCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;

layout(location = 0) out vec2 texCoord;

void main() {
    texCoord = uvCoord;
    texCoord.y *= 1.0; // Flip Y coordinate if needed
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
}
