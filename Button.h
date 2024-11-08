#pragma once

#include <string>
#include <glew.h>
#include <glm/glm.hpp>
#include <SDL.h>

class Button 
{
public:
	/// <summary>
	/// constructor
	/// </summary>
	/// <param name="image">image file</param>
	/// <param name="pos">position of button</param>
	/// <param name="size">size of button</param>
	Button(const std::string& image, const glm::vec2& pos, const glm::vec2& size);
	
	/// <summary>
	/// destructor
	/// </summary>
	~Button();

	/// <summary>
	/// renders the button
	/// </summary>
	void Render();

	/// <summary>
	/// if the mouse is hovered over button
	/// </summary>
	/// <param name="mousePos">the mouse position</param>
	/// <returns>if hovered</returns>
	bool IsHovered(const glm::vec2& mousePos);

private:
	GLuint textureID;
	glm::vec2 pos;
	glm::vec2 size;
	bool hovered;

	/// <summary>
	/// loads the texture
	/// </summary>
	/// <param name="image">image file</param>
	/// <returns>true if it binds to the texture id</returns>
	bool LoadTexture(const std::string& image);
};