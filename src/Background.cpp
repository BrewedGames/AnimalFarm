#include "Background.h"
#include <stb_image.h>
#include <MMath.h>
using namespace MATH;


Background::Background() 
{
	pos = Vec3(0.0f, 0.0f, 0.0f);
}
Background::~Background() {}

bool Background::OnCreate()
{
	SetupQuad();
	return true;
}


void Background::Update(float deltaTime)
{
	modelMatrix = MMath::translate(pos) * MMath::scale(Vec3(image_width, image_height, 1.0f));
}

void Background::SetupQuad()
{

    GLfloat vertices[] = {
        // positions         // texture coordinates
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top right
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom left
       -0.5f,  0.5f, 0.0f, 0.0f, 0.0f  // Top left
    };

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

bool Background::LoadSprite(const char *filename, float _width, float _height, Vec3 _pos)
{

	image_width = _width;
	image_height = _height;
	pos = _pos;
	int width, height, channels;
	unsigned char *image_data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (image_data == nullptr)
	{
		std::cerr << "Failed to load texture" << std::endl;
		return false;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image_data);
	return true;
}

void Background::Render() const
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
