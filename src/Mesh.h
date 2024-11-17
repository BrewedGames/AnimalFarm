#ifndef MESH_H
#define MESH_H

#include <vector>
#include <VMath.h>
#include <Vector.h>
#include <string>
#include <glew.h>

using namespace MATH;

class Mesh
{

	Mesh(const Mesh &) = delete;
	Mesh(Mesh &&) = delete;
	Mesh &operator=(const Mesh &) = delete;
	Mesh &operator=(Mesh &&) = delete;

private:
	const char *filename;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;
	size_t dataLength;
	GLenum drawmode;

	Vec3 mappedColors = Vec3(0.0f, 0.0f, 0.0f);
	void LoadModel(const char *filename);
	void StoreMeshData(GLenum drawmode_);
	GLuint vao, vbo;

public:
	Mesh(const char *filename_);
	~Mesh();
	void Render() const;
	void Render(GLenum drawmode) const;
	void setMappedColors(Vec3 colors);
};

#endif