#ifndef ECSCOMPONENTS_H
#define ECSCOMPONENTS_H
#include "ECS.h"
#include <SDL.h>
#include <SDL_image.h>
#include <Vector.h>
#include "Window.h"
#include <string>
#include <glew.h>
#include <Matrix.h>

#include <cute_c2.h>
#include <cmath>
#include <Matrix.h>
#include <MMath.h>
#include <QMath.h>
#include <VMath.h>
#include "SimpleShape.h"
#include <vector>
#include <map>
#include <simpleini.h>

#include <SDL_mixer.h>

#include "Shader.h"
#include "Camera.h"

#include "Mesh.h"
#include "Texture.h"

class Mesh;
class Texture;

using namespace MATH;

class AudioComponent : public ECSComponent
{

public:
	AudioComponent();
	~AudioComponent();
	bool OnCreate() { return true; };
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;

	void Play();
	void Stop();
	void Pause();
	void Resume();
	void SetVolume(float vol);
	float GetVolume() { return volume; };
	void SetLoop(bool loop) { isLoop = loop; };
	void setAudio(const char *_filename, bool _isMusic = true);

private:
	struct Audio
	{
		enum class Type
		{
			Music,
			Sound
		};

		Type type;
		union
		{
			Mix_Music *music;
			Mix_Chunk *sound;
		};

		Audio() : type(Type::Music) {}
	};

	Audio audio;

	float volume = 100.0f;
	bool isMusic = true;
	bool isPlaying = false;
	bool isPaused = false;
	bool isLoop = false;
	const char *filename;
};

class SpriteComponent : public ECSComponent
{

private:
	float image_width, image_height;
	const char *filename;
	GLuint VAO, VBO, EBO, texture;
	Vec3 pos;
	Matrix4 modelMatrix;

	bool isAnimated = false;
	int speed = 100; // in milliseconds higher is slower
	int columns = 0; // top to bottom
	int rows = 0;	 // left to right
	int sx = 0, sy = 0;

	int animationTime = 0;

	int currentFrame = 0;
	int totalFrames = 0;
	int framesPerRow = 0;
	float frameDuration = 0.1f;
	float timeSinceLastFrame = 0.0f;

	int AnimationStartFrame = 0;
	int AnimationEndFrame = 0;
	int AnimationSpeed = 0;

	Camera cam;

	Shader *animatedShader;
	Shader *spriteShader;

	struct Animation
	{
		int StartFrame;
		int EndFrame;
		int Speed;
	};

	std::map<std::string, Animation> Animations;
	std::vector<Animation> AnimationList;

public:
	SpriteComponent();
	~SpriteComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;
	void SetupQuad();

	void SetAnimation(const char *_name, int _startFrame, int _endFrame, int _speed)
	{
		Animations[_name] = Animation{_startFrame, _endFrame, _speed};
	}
	void PlayAnimation(const char *_name)
	{

		AnimationList.push_back(Animations[_name]);
	};
	void ClearAnimation() { AnimationList.clear(); };
	bool LoadSprite(const char *filename, float _width, float _height, Vec3 _pos, bool _isAnimated = false, int _totalFrames = 0, int _framesPerRow = 0, int _speed = 100, Camera _cam = Camera());
	Matrix4 GetModelMatrix() { return modelMatrix; };
	Vec3 getPos() { return pos; };
	void setPos(Vec3 _pos) { pos = _pos; };
	void setPosX(float _x) { pos.x = _x; };
	void setPosY(float _y) { pos.y = _y; };
	void setPosZ(float _z) { pos.z = _z; };
	float X() { return pos.x; };
	float Y() { return pos.y; };
	float Z() { return pos.z; };
	const char *GetSpritePath() { return filename; };
};

class ColliderComponent : public ECSComponent
{
private:
	std::map<ColliderComponent *, char *> Tags;
	char Tag;
	SimpleShape shapeRenderer;
	float WindowWidth;
	float WindowHeight;
	CSimpleIniA ini;
	Vec3 Red = Vec3(1.0f, 0.0f, 0.0f);
	Vec3 Green = Vec3(0.0f, 1.0f, 0.0f);
	bool hasCollided = false;
	Vec2 Dimensions;
	Vec3 pos;

	struct Collider
	{
		enum class Type
		{
			None,
			Circle,
			AABB,
			Capsule,
			Polygon
		};

		Type type;
		union
		{
			c2Circle circle;
			c2AABB aabb;
			c2Capsule capsule;
			c2Poly poly;
		};

		Collider() : type(Type::None) {}
	};

	Collider collider;

public:
	ColliderComponent();
	~ColliderComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;
	std::map<ColliderComponent *, char *> getTags() { return Tags; };
	std::map<ColliderComponent *, char *> &setTags(std::map<ColliderComponent *, char *> &_Tags)
	{
		Tags = _Tags;
		return Tags;
	} // should not make a new vector hopefully
	char getTag() { return Tag; };
	void setTag(char _Tag) { Tag = _Tag; };
	Vec3 getPos() { return pos; };
	Vec3 setPos(Vec3 _pos);
	void AddCircleCollider(float x, float y, float r);
	void AddAABBCollider(float x, float y, float width, float height);
	void AddCapsuleCollider(float x, float y, float width, float height);
	void AddPolygonCollider(float x, float y, float width, float height);
	// Collider::Type getColliderType() { return collider.type; };
	int getColliderType() const { return static_cast<int>(collider.type); }
	bool isColliding(ColliderComponent *other);
	bool isCollidingWithTag(char tag);
};

class MeshComponent : public ECSComponent
{

private:
	const char *mesh_filename;
	const char *texture_filename;
	Mesh *mesh;
	Texture *texture;
	Shader *UNLIT_shader;
	Camera cam;
	bool hasTexture = true;
	Vec3 _pos;
	Vec3 _scale;
	Vec3 _rotation;
	Matrix4 _transform;
	Matrix4 combinedRotation;

public:
	MeshComponent();
	~MeshComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;
	void Render(GLenum drawmode) const;

	void loadMesh(const char *mesh_filename_, const char *texture_filename_, Vec3 pos = Vec3(0.0f, 0.0f, 0.0f),
						  Vec3 scale = Vec3(50.0f, 50.0f, 50.0f),
						  Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f));
	Vec3 pos() { return _pos; }
	Vec3 scale() { return _scale; }
	Vec3 rotation() { return _rotation; }
	Matrix4 transform() { return _transform; }

	void initTransform()
	{
		_transform =  MMath::translate(_pos) * combinedRotation * MMath::scale(_scale);
	}
	void setPos(Vec3 pos) { _pos = pos; }
	void setScale(Vec3 scale) { _scale = scale;  }
	void setRotation(Vec3 rotation) {
		 _rotation = rotation; 
	Matrix4 rotationMatrixX = MMath::rotate(rotation.x, Vec3(0.0f, 1.0f, 0.0f));
    Matrix4 rotationMatrixY = MMath::rotate(rotation.y, Vec3(1.0f, 0.0f, 0.0f)); 
    Matrix4 rotationMatrixZ = MMath::rotate(rotation.z, Vec3(0.0f, 0.0f, 1.0f)); 

	combinedRotation = rotationMatrixZ * rotationMatrixY * rotationMatrixX;
	}
	void setMappedColors(Vec3 colors) { mesh->setMappedColors(colors); };
	inline GLuint getTextureID() const { return texture->getTextureID(); }
	inline int getImageWidth() const { return texture->getImageWidth(); }
	inline int getImageHeight() const { return texture->getImageHeight(); }
};





class ShaderComponent : public ECSComponent
{
	ShaderComponent(const ShaderComponent &) = delete;
	ShaderComponent(ShaderComponent &&) = delete;
	ShaderComponent &operator=(const ShaderComponent &) = delete;
	ShaderComponent &operator=(ShaderComponent &&) = delete;

private:
	const char *vertFilename;
	const char *fragFilename;
	const char *tessCtrlFilename;
	const char *tessEvalFilename;
	const char *geomFilename;

	GLuint shaderID;
	GLuint vertShaderID;
	GLuint fragShaderID;
	GLuint tessCtrlShaderID;
	GLuint tessEvalShaderID;
	GLuint geomShaderID;
	std::unordered_map<std::string, GLuint> uniformMap;

	/// Private helper methods
	char *ReadTextFile(const char *fileName);
	bool CompileAttach();
	bool Link();
	void SetUniformLocations();

public:
	ShaderComponent(ECSComponent *parent_, const char *vertFilename_, const char *fragFilename_,
					const char *tessCtrlFilename_ = nullptr, const char *tessEvalFilename = nullptr,
					const char *geomFilename_ = nullptr);
	~ShaderComponent();
	bool OnCreate() override;
	void OnDestroy() override;

	void Update(float deltaTime) override; // just cause

	inline GLuint GetProgram() const { return shaderID; }
	GLuint GetUniformID(std::string name);

	virtual void Render() const;
};

/*

using dynamic collisions as follows
void setCircle(float x, float y, float radius) {
		collider.type = Collider::Type::Circle;
		collider.circle.p = c2V(x, y);
		collider.circle.r = radius;
	}


*/

//
// class BodyComponent : public ECSComponent {
//
//   private:
//   		Vec3 pos;
//		Vec3 vel;
//		Vec3 acc;
//        float mass = 1.0f;
//   public:
//    BodyComponent();
//    ~BodyComponent();
//    bool OnCreate(); // reminder to implement a check OnCreate if Entity has a Texture set width and height relatively - zoe
//	void OnDestroy();
//	void Update(float deltaTime);
//    void Render() const;
//    void TestFunction();
//
//    Vec3 getPos() { return pos; };
//    Vec3 getVel() { return vel; };
//    Vec3 getAcc() { return acc; };
//
//    float getMass() { return mass; };
//
//    void setPos(Vec3 p) { pos = p; };
//    void setVel(Vec3 v) { vel = v; };
//    void setAcc(Vec3 a) { acc = a; };
//    void setMass(float m) { mass = m; };
//
//    void ApplyForce(Vec3 force);
//
//};

#endif