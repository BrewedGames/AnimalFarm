#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>
#include <SDL_image.h>

class Texture
{

public:
	Texture();
	~Texture();

	bool LoadTexture(const char *filename);
	inline GLuint getTextureID() const { return textureID; }
	inline int getImageWidth() const { return image_width; }
	inline int getImageHeight() const { return image_height; }

private:
	GLuint textureID;
	int image_width, image_height;
};

#endif