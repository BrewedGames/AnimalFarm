#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <glew.h>
#include <Matrix.h>
#include <simpleini.h>
#include <VMath.h>
using namespace MATH;

class Background {
    private:
	float image_width, image_height;
	unsigned char *image_data;
	GLuint VAO, VBO, EBO, texture;
	Vec3 pos;
	Matrix4 modelMatrix;


public:
    Background();
    ~Background();
	bool OnCreate();
	void Update(float deltaTime);
	void Render() const;
	void SetupQuad();
	bool LoadSprite(const char *filename, float _width, float _height, Vec3 _pos);
	Matrix4 GetModelMatrix() { return modelMatrix; };
	Vec3 getPos() { return pos; };
	void setPos(Vec3 _pos) { pos = _pos; };
	void setPosX(float _x) { pos.x = _x; };
	void setPosY(float _y) { pos.y = _y; };
	void setPosZ(float _z) { pos.z = _z; };
	float X() { return pos.x; };
	float Y() { return pos.y; };
	float Z() { return pos.z; };
   public:

};

#endif