#version 450

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D spriteTexture;
uniform int currentFrame;
uniform int framesPerRow;
uniform int totalFrames;

void main() {
    int row = currentFrame / framesPerRow;
    int col = currentFrame % framesPerRow;
    
    float frameWidth = 1.0 / float(framesPerRow);
    float frameHeight = 1.0 / float((totalFrames + framesPerRow - 1) / framesPerRow); 
    
    vec2 frameOrigin = vec2(float(col) * frameWidth, float(row) * frameHeight);
    vec2 uv = frameOrigin + TexCoord * vec2(frameWidth, frameHeight);
    
    FragColor = texture(spriteTexture, uv);
}