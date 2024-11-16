#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texCoord;

uniform sampler2D myTexture;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 textureColor = texture(myTexture, texCoord);
    fragColor = textureColor;
}
