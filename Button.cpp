#include "Button.h"
#include <SDL_image.h>
#include <iostream>

Button::Button(const std::string& image, const glm::vec2& pos, 
	const glm::vec2& size): pos(pos), size(size), hovered(false)
{
	LoadTexture(image);
}

Button::~Button()
{
	glDeleteTextures(1, &textureID);
}

void Button::Render()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);

	//gives a tint if the button is hovered with the default color no tint
	glm::vec4 color = hovered ? glm::vec4(0.8f, 0.8f, 0.8f, 1.0f) : glm::vec4(1.0f);
	glColor4f(color.r, color.g, color.b, color.a);

	glTexCoord2f(0.0f, 0.0f);//top left coord
	glVertex2f(pos.x, pos.y);//top left vert
	glTexCoord2f(1.0f, 0.0f);//top right coord
	glVertex2f(pos.x + size.x, pos.y);//top right vert
	glTexCoord2f(1.0f, 1.0f);//bottom right coord
	glVertex2f(pos.x + size.x, pos.y + size.y);//bottom right vert
	glTexCoord2f(0.0f, 1.0f);//bottom left coord
	glVertex2f(pos.x, pos.y + size.y);//bottom left vert

	glEnd();
}

bool Button::IsHovered(const glm::vec2& mousePos)
{
	//check the bounds
	hovered = (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x 
		&& mousePos.y >= pos.y && mousePos.y <= pos.y + size.y);

	return hovered;
}

bool Button::LoadTexture(const std::string& image)
{
	SDL_Surface* surface = IMG_Load(image.c_str());

	if (!surface) 
	{
		std::cerr << image << "wont load\n";
		return false;
	}

	glGenTextures(1, &textureID);//new texture id
	glBindTexture(GL_TEXTURE_2D, textureID);//bind

	//transfer
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return true;

}
