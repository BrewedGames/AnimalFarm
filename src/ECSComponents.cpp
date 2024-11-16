#define STB_IMAGE_IMPLEMENTATION
#define CUTE_C2_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "ECSComponents.h"

#include <cute_c2.h>
#include <stb_image.h>
#include <fstream>
#include <string.h>
#include <SDL_image.h>
#include <MMath.h>

namespace fs = std::filesystem;
using namespace MATH;




// BodyComponent::BodyComponent(): ECSComponent(nullptr){
// }
//
// BodyComponent::~BodyComponent() {}
//
//
// bool BodyComponent::OnCreate() {
//	return true;
// }
//
// void BodyComponent::OnDestroy() {
// }
//
// void BodyComponent::Update(float deltaTime) {
//
// }
//
// void BodyComponent::Render() const {
// }
//
//

AudioComponent::AudioComponent() : ECSComponent(nullptr)
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}
}
AudioComponent::~AudioComponent()
{
	isMusic ? Mix_FreeMusic(audio.music) : Mix_FreeChunk(audio.sound);
	if (isMusic)
	{
		audio.music = nullptr;
	}
	else
	{
		audio.sound = nullptr;
	}
	filename = nullptr;
}
void AudioComponent::setAudio(const char *_filename, bool _isMusic)
{
	filename = _filename;
	isMusic = _isMusic;

	isMusic ? audio.type = Audio::Type::Music : audio.type = Audio::Type::Sound;
	if (isMusic)
	{
		audio.music = Mix_LoadMUS(filename);
		if (audio.music == nullptr)
		{
			std::cout << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
		}
	}
	else
	{
		audio.sound = Mix_LoadWAV(filename);
		if (audio.sound == nullptr)
		{
			std::cout << "Failed to load sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
		}
	}
}
void AudioComponent::Play()
{
	isMusic ? Mix_PlayMusic(audio.music, isLoop ? -1 : 0) : Mix_PlayChannel(-1, audio.sound, isLoop ? -1 : 0);
}
void AudioComponent::Stop()
{
	isMusic ? Mix_HaltMusic() : Mix_HaltChannel(-1);
}
void AudioComponent::Pause()
{
	isMusic ? Mix_PauseMusic() : Mix_Pause(-1);
}
void AudioComponent::Resume()
{
	isMusic ? Mix_ResumeMusic() : Mix_Resume(-1);
}
void AudioComponent::SetVolume(float vol)
{
	volume = vol;
	if (volume < 0.0f)
		volume = 0.0f;
	isMusic ? Mix_VolumeMusic(vol) : Mix_Volume(-1, vol);
}

void AudioComponent::Update(float deltaTime) {}
void AudioComponent::Render() const {}
void AudioComponent::OnDestroy() {}

SpriteComponent::SpriteComponent() : ECSComponent(nullptr)
{
	pos = Vec3(0.0f, 0.0f, 0.0f);
}
SpriteComponent::~SpriteComponent() {}

bool SpriteComponent::OnCreate()
{
	SetupQuad();
	animatedShader = new Shader("./src/shaders/SpriteAnimations.vert", "./src/shaders/SpriteAnimations.frag");
	if (animatedShader->OnCreate() == false)
	{
		std::cout << "Shader failed ... we have a problem\n";
	}

	spriteShader = new Shader("./src/shaders/DefaultTextureShader.vert", "./src/shaders/DefaultTextureShader.frag");
	if (spriteShader->OnCreate() == false)
	{
		std::cout << "Shader failed ... we have a problem\n";
	}

	return true;
}

void SpriteComponent::OnDestroy() {}
void SpriteComponent::Update(float deltaTime)
{
	modelMatrix = MMath::translate(pos) * MMath::scale(Vec3(image_width, image_height, 1.0f));

	if (isAnimated)
	{
		if (!AnimationList.empty())
		{
			timeSinceLastFrame += deltaTime;
			if (timeSinceLastFrame >= frameDuration)
			{
				// currentFrame = (currentFrame + 1) % totalFrames;  // Loop through all frames
				// currentFrame = (currentFrame + 1) % (15 - 8 + 1) + 8; // loop through frames 8 to 15
				currentFrame = (currentFrame + 1) % (AnimationList.back().EndFrame - AnimationList.back().StartFrame + 1) + AnimationList.back().StartFrame;
				timeSinceLastFrame = 0.0f;
			}
		}
	}
}

void SpriteComponent::SetupQuad()
{

	GLfloat vertices[] = {
		// positions         // texture coordinates
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,	// Top right
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f,	// Bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom left
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f	// Top left
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

bool SpriteComponent::LoadSprite(const char *_filename, float _width, float _height, Vec3 _pos, bool _isAnimated, int _totalFrames, int _framesPerRow, int _speed, Camera _cam)
{

	isAnimated = _isAnimated;
	image_width = _width;
	image_height = _height;
	filename = _filename;
	cam = _cam;
	pos = _pos;
	int width, height, channels;
	unsigned char *image_data = stbi_load(_filename, &width, &height, &channels, STBI_rgb_alpha);
	if (image_data == nullptr)
	{
		std::cerr << "Failed to load texture" << std::endl;
		return false;
	}

	std::cout << "Texture ID for entity: " << texture << " " << filename << std::endl;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLfloat borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (isAnimated)
	{
		framesPerRow = _framesPerRow;
		totalFrames = _totalFrames;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image_data);

	return true;
}

void SpriteComponent::Render() const
{

	isAnimated ? glUseProgram(animatedShader->GetProgram()) : glUseProgram(spriteShader->GetProgram());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha blending
	glDisable(GL_DEPTH_TEST);

	isAnimated ? glUniformMatrix4fv(animatedShader->GetUniformID("projection"), 1, GL_FALSE, cam.GetProjectionMatrix()) : glUniformMatrix4fv(spriteShader->GetUniformID("projection"), 1, GL_FALSE, cam.GetProjectionMatrix());
	isAnimated ? glUniformMatrix4fv(animatedShader->GetUniformID("view"), 1, GL_FALSE, cam.GetViewMatrix()) : glUniformMatrix4fv(spriteShader->GetUniformID("view"), 1, GL_FALSE, cam.GetViewMatrix());
	isAnimated ? glUniformMatrix4fv(animatedShader->GetUniformID("model"), 1, GL_FALSE, modelMatrix) : glUniformMatrix4fv(spriteShader->GetUniformID("model"), 1, GL_FALSE, modelMatrix);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (isAnimated)
	{
		glUniform1i(animatedShader->GetUniformID("currentFrame"), currentFrame);
		glUniform1i(animatedShader->GetUniformID("framesPerRow"), framesPerRow);
		glUniform1i(animatedShader->GetUniformID("totalFrames"), totalFrames);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

ColliderComponent::ColliderComponent() : ECSComponent(nullptr)
{
	ini.SetUnicode();
	ini.LoadFile("config.ini");
	WindowWidth = std::stoi(ini.GetValue("Display", "width"));
	WindowHeight = std::stoi(ini.GetValue("Display", "height"));
	SimpleShape::setWindowDimensions(WindowWidth, WindowHeight);
}
ColliderComponent::~ColliderComponent() {}

bool ColliderComponent::OnCreate() { return true; }
void ColliderComponent::OnDestroy() {}
void ColliderComponent::Update(float deltaTime) {}
void ColliderComponent::Render() const
{
	switch (collider.type)
	{
	case Collider::Type::Circle:
		shapeRenderer.simpleCircle(hasCollided ? Red : Green, Vec3(collider.circle.p.x, collider.circle.p.y, 0.0f), collider.circle.r, 32, false);
		break;

	case Collider::Type::Capsule:
		shapeRenderer.simpleCapsule(hasCollided ? Red : Green, Vec3(pos.x, pos.y, 0.0f), Dimensions.x, Dimensions.y, 32, false);
		break;

	case Collider::Type::AABB:
	{
		std::vector<Vec3> pointsVec;
		pointsVec.push_back(Vec3(collider.aabb.min.x, collider.aabb.min.y, 0.0f));
		pointsVec.push_back(Vec3(collider.aabb.min.x, collider.aabb.max.y, 0.0f));
		pointsVec.push_back(Vec3(collider.aabb.max.x, collider.aabb.max.y, 0.0f));
		pointsVec.push_back(Vec3(collider.aabb.max.x, collider.aabb.min.y, 0.0f));
		shapeRenderer.simpleQuad(hasCollided ? Red : Green, pointsVec, false);
		break;
	}
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}

Vec3 ColliderComponent::setPos(Vec3 _pos)
{
	pos = _pos;

	switch (collider.type)
	{
	case Collider::Type::Circle:
		collider.circle.p = c2V(_pos.x, _pos.y);
		break;
	case Collider::Type::Capsule:
	{
		float halfHeight = (Dimensions.y - Dimensions.x) / 2.0f;
		float radius = Dimensions.x / 2.0f;
		collider.capsule.a = c2V(_pos.x, _pos.y - halfHeight);
		collider.capsule.b = c2V(_pos.x, _pos.y + halfHeight);
		break;
	}
	case Collider::Type::AABB:

		collider.aabb.min = c2V(_pos.x, _pos.y);
		collider.aabb.max = c2V(_pos.x + Dimensions.x, _pos.y + Dimensions.y);
		break;
	}
	return _pos;
}

void ColliderComponent::AddCircleCollider(float x, float y, float r)
{
	collider.type = Collider::Type::Circle;
	collider.circle.p = c2V(x, y);
	collider.circle.r = r;
	pos.x = x;
	pos.y = y;
}
void ColliderComponent::AddAABBCollider(float x, float y, float width, float height)
{

	collider.type = Collider::Type::AABB;
	Dimensions = Vec2(width, height);
	pos.x = x;
	pos.y = y;
	collider.aabb.min = c2V(x, y);
	collider.aabb.max = c2V(x + width, y + height);
}
void ColliderComponent::AddCapsuleCollider(float x, float y, float width, float height)
{

	collider.type = Collider::Type::Capsule;
	Dimensions = Vec2(width, height);
	pos.x = x;
	pos.y = y;
	float halfHeight = (height - width) / 2.0f;
	float radius = width / 2.0f;

	collider.capsule.a = c2V(x, y - halfHeight);
	collider.capsule.b = c2V(x, y + halfHeight);

	collider.capsule.r = radius;
}
void ColliderComponent::AddPolygonCollider(float x, float y, float width, float height) {}
bool ColliderComponent::isColliding(ColliderComponent *other)
{

	switch (other->getColliderType())
	{
	case Collider::Type::Circle:
		switch (collider.type)
		{
		case Collider::Type::Circle:
			hasCollided = c2CircletoCircle(collider.circle, other->collider.circle);
			return hasCollided;
		case Collider::Type::Capsule:
			hasCollided = c2CircletoCapsule(other->collider.circle, collider.capsule);
			return hasCollided;
		case Collider::Type::AABB:
			hasCollided = c2CircletoAABB(other->collider.circle, collider.aabb);
			return hasCollided;
		}
		break;
	}
	return false;
}
bool isCollidingWithTag(char *tag) { return false; }


Transform3DComponent::Transform3DComponent(Vec3 pos, Vec3 scale, Quaternion rotation, Transform3DComponent* _parent):  _pos(pos), _scale(scale), _rotation(rotation),ECSComponent(nullptr) {
	_pos = pos;
	_scale = scale;
	_rotation = rotation;
	initTransform(_parent);
}

Transform3DComponent::~Transform3DComponent() {}

void Transform3DComponent::OnDestroy() {}

void Transform3DComponent::Update(float deltaTime) {}

void Transform3DComponent::Render() const {}

MeshComponent::MeshComponent(ECSComponent* parent_, const char* filename_) : ECSComponent(parent_) {
    filename = filename_;
	LoadModel(filename);
    StoreMeshData(GL_TRIANGLES);

}
MeshComponent::~MeshComponent() {}

bool MeshComponent::OnCreate() {
 return true;
}


void MeshComponent::LoadModel(const char* filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    vertices.clear();
    normals.clear();
    uvCoords.clear();

	try {
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
			throw std::runtime_error(warn + err);
		}
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Failed to load OBJ file: " << e.what() << std::endl;
	}

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vec3 vertex{};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];

            Vec3 normal{};
            normal.x = attrib.normals[3 * index.normal_index + 0];
            normal.y = attrib.normals[3 * index.normal_index + 1];
            normal.z = attrib.normals[3 * index.normal_index + 2];

            Vec2 uvCoord{};
            uvCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
            uvCoord.y = -attrib.texcoords[2 * index.texcoord_index + 1];

            vertices.push_back(vertex);
            normals.push_back(normal);
            uvCoords.push_back(uvCoord);
        }
    }
}

void MeshComponent::StoreMeshData(GLenum drawmode_) {
    drawmode = drawmode_;
    /// These just make the code easier for me to read
#define VERTEX_LENGTH 	(vertices.size() * (sizeof(Vec3)))
#define NORMAL_LENGTH 	(normals.size() * (sizeof(Vec3)))
#define TEXCOORD_LENGTH (uvCoords.size() * (sizeof(Vec2)))

    const int verticiesLayoutLocation = 0;
    const int normalsLayoutLocation = 1;
    const int uvCoordsLayoutLocation = 2;

    /// create and bind the VOA
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    /// Create and initialize vertex buffer object VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH + TEXCOORD_LENGTH, nullptr, GL_STATIC_DRAW);

    /// assigns the addr of "points" to be the beginning of the array buffer "sizeof(points)" in length
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_LENGTH, &vertices[0]);
    /// assigns the addr of "normals" to be "sizeof(points)" offset from the beginning and "sizeof(normals)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH, NORMAL_LENGTH, &normals[0]);
    /// assigns the addr of "texCoords" to be "sizeof(points) + sizeof(normals)" offset from the beginning and "sizeof(texCoords)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH, TEXCOORD_LENGTH, &uvCoords[0]);

    glEnableVertexAttribArray(verticiesLayoutLocation);
    glVertexAttribPointer(verticiesLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(normalsLayoutLocation);
    glVertexAttribPointer(normalsLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH));
    glEnableVertexAttribArray(uvCoordsLayoutLocation);
    glVertexAttribPointer(uvCoordsLayoutLocation, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH + NORMAL_LENGTH));


    dataLength = vertices.size();

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();
    normals.clear();
    uvCoords.clear();

    /// Don't need these defines sticking around anymore
#undef VERTEX_LENGTH
#undef NORMAL_LENGTH
#undef TEXCOORD_LENGTH

}

void MeshComponent::Render() const {
    glBindVertexArray(vao);
    glDrawArrays(drawmode, 0, dataLength);
    glBindVertexArray(0); // Unbind the VAO
}

void MeshComponent::Render(GLenum drawmode_) const {
    glBindVertexArray(vao);
    glDrawArrays(drawmode_, 0, dataLength);
    glBindVertexArray(0); // Unbind the VAO
}

void MeshComponent::OnDestroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

/// Currently unused.

void MeshComponent::Update(const float deltaTime) {}


void MeshComponent::setMappedColors(Vec3 colors) {
	mappedColors = colors;
	
}


TextureComponent::TextureComponent():textureID(0), ECSComponent(nullptr) {}
void TextureComponent::OnDestroy() {}
void TextureComponent::Update(float deltaTime) {}
void TextureComponent::Render() const {}

bool TextureComponent::LoadTexture(const char* filename) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	SDL_Surface *textureSurface = IMG_Load(filename);
	if (textureSurface == nullptr) {
		return false;
	}
	image_height = textureSurface->h; image_width = textureSurface->w;
	int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	
	SDL_FreeSurface(textureSurface);
	/// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0); /// Unbind the texture
	return true;
}


TextureComponent::~TextureComponent(){
	glDeleteTextures(1, &textureID);
}


ShaderComponent::ShaderComponent(ECSComponent *parent_, const char *vertFilename_, const char *fragFilename_,
								 const char *tessCtrlFilename_, const char *tessEvalFilename_,
								 const char *geomFilename_) : ECSComponent(parent_),
															  shaderID(0), vertShaderID(0), fragShaderID(0), tessCtrlShaderID(0),
															  tessEvalShaderID(0), geomShaderID(0),
															  vertFilename(vertFilename_),
															  fragFilename(fragFilename_),
															  tessCtrlFilename(tessCtrlFilename_),
															  tessEvalFilename(tessEvalFilename_),
															  geomFilename(geomFilename_) {}

ShaderComponent::~ShaderComponent() {}

bool ShaderComponent::OnCreate()
{
	bool status;
	status = CompileAttach();
	if (status == false)
		return false;
	status = Link();
	if (status == false)
		return false;

	SetUniformLocations();
	return true;
}

void ShaderComponent::OnDestroy()
{

	glDetachShader(shaderID, fragShaderID);
	glDetachShader(shaderID, vertShaderID);
	glDeleteShader(fragShaderID);
	glDeleteShader(vertShaderID);

	if (tessCtrlShaderID)
	{
		glDetachShader(shaderID, tessCtrlShaderID);
		glDeleteShader(tessCtrlShaderID);
	}
	if (tessEvalShaderID)
	{
		glDetachShader(shaderID, tessEvalShaderID);
		glDeleteShader(tessEvalShaderID);
	}

	if (geomShaderID)
	{
		glDetachShader(shaderID, geomShaderID);
		glDeleteShader(geomShaderID);
	}

	glDeleteProgram(shaderID);
}

bool ShaderComponent::CompileAttach()
{
	GLint status;
	const char *vertText = nullptr;
	const char *fragText = nullptr;
	const char *tessCtrlText = nullptr;
	const char *tessEvalText = nullptr;
	const char *geomText = nullptr;

	try
	{
		vertText = ReadTextFile(vertFilename);
		fragText = ReadTextFile(fragFilename);
		if (vertText == nullptr || fragText == nullptr)
		{
			return false;
		}
		/// GL_VERTEX_SHADER and GL_FRAGMENT_SHADER are defined in glew.h
		vertShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		/// Check for errors
		if (vertShaderID == 0 || fragShaderID == 0)
		{
			std::string errorMsg("Can't create a new shader");
			throw errorMsg;
		}

		glShaderSource(vertShaderID, 1, &vertText, 0);
		glShaderSource(fragShaderID, 1, &fragText, 0);

		glCompileShader(vertShaderID);
		/// Check for errors
		glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			GLsizei errorLogSize = 0;
			GLsizei titleLength;
			std::string errorLog = vertFilename;
			errorLog += "\nVERT:\n";
			titleLength = errorLog.length();
			glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize(titleLength + errorLogSize);
			glGetShaderInfoLog(vertShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
			throw errorLog;
		}

		glCompileShader(fragShaderID);
		/// Check for errors
		glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			GLsizei errorLogSize = 0;
			GLsizei titleLength;
			std::string errorLog = fragFilename;
			errorLog += "\nFrag:\n";
			titleLength = errorLog.length();
			glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize(titleLength + errorLogSize);
			glGetShaderInfoLog(fragShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
			throw errorLog;
		}

		if (tessCtrlFilename != nullptr && tessEvalFilename != nullptr)
		{
			tessCtrlText = ReadTextFile(tessCtrlFilename);
			tessEvalText = ReadTextFile(tessEvalFilename);

			if (tessCtrlText == nullptr || tessEvalText == nullptr)
			{
				std::string errorLog = "Can't read either tessCtrlFile or tessEvalFile";
				throw errorLog;
			}
			tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
			tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

			glShaderSource(tessCtrlShaderID, 1, &tessCtrlText, 0);
			glShaderSource(tessEvalShaderID, 1, &tessEvalText, 0);

			glCompileShader(tessCtrlShaderID);
			/// Check for errors
			glGetShaderiv(tessCtrlShaderID, GL_COMPILE_STATUS, &status);
			if (status == 0)
			{
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = tessCtrlFilename;
				errorLog += "\nTESS CTRL:\n";
				titleLength = errorLog.length();
				glGetShaderiv(tessCtrlShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize(titleLength + errorLogSize);
				glGetShaderInfoLog(tessCtrlShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}

			glCompileShader(tessEvalShaderID);
			/// Check for errors
			glGetShaderiv(tessEvalShaderID, GL_COMPILE_STATUS, &status);
			if (status == 0)
			{
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = tessEvalFilename;
				errorLog += "\nTESS EVAL:\n";
				titleLength = errorLog.length();
				glGetShaderiv(tessEvalShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize(titleLength + errorLogSize);
				glGetShaderInfoLog(tessEvalShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}
		}
		else if ((tessCtrlFilename != nullptr) != (tessEvalFilename != nullptr))
		{ /// XOR
			std::string errorLog = "To use tessilation there needs to be both a tessCtrlFilename and tessEvalFilename";
			throw errorLog;
		}

		if (geomFilename != nullptr)
		{
			geomText = ReadTextFile(geomFilename);
			if (geomText == nullptr)
			{
				std::string errorMsg("Can't open file:");
				throw errorMsg + geomFilename;
			}
			geomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
			if (geomShaderID == 0)
			{
				std::string errorMsg("Can't create a new geom shader ID");
				throw errorMsg;
			}
			glShaderSource(geomShaderID, 1, &geomText, 0);
			glCompileShader(geomShaderID);
			/// Check for errors
			glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &status);
			if (status == 0)
			{
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = geomFilename;
				errorLog += "\nGEOM:\n";
				titleLength = errorLog.length();
				glGetShaderiv(geomShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize(titleLength + errorLogSize);
				glGetShaderInfoLog(geomShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}
		}
		shaderID = glCreateProgram();
		glAttachShader(shaderID, fragShaderID);
		glAttachShader(shaderID, vertShaderID);
		if (tessCtrlShaderID)
			glAttachShader(shaderID, tessCtrlShaderID);
		if (tessEvalShaderID)
			glAttachShader(shaderID, tessEvalShaderID);
		if (geomShaderID)
			glAttachShader(shaderID, geomShaderID);

		if (vertText)
			delete[] vertText;
		if (fragText)
			delete[] fragText;
		if (tessCtrlText)
			delete[] tessCtrlText;
		if (tessEvalText)
			delete[] tessEvalText;
		if (geomText)
			delete[] geomText;
	}
	catch (std::string error)
	{
		printf("%s\n", &error[0]);
		// Debug::Error(error, __FILE__, __LINE__);
		return false;
	}
	return true;
}

bool ShaderComponent::Link()
{
	GLint status;
	try
	{
		glLinkProgram(shaderID);
		/// Check for errors
		glGetProgramiv(shaderID, GL_LINK_STATUS, &status);
		if (status == 0)
		{
			GLsizei errorLogSize = 0;
			std::string errorLog;
			glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize(errorLogSize);
			glGetProgramInfoLog(shaderID, errorLogSize, &errorLogSize, &errorLog[0]);
			throw errorLog;
		}
	}
	catch (std::string error)
	{
		// Debug::Error(error, __FILE__, __LINE__);
		return false;
	}
	return true;
}

GLuint ShaderComponent::GetUniformID(std::string name)
{

	auto id = uniformMap.find(name);
#ifdef _DEBUG
	static bool writeBadUniformWarning = true;
	if (id == uniformMap.end())
	{
		if (writeBadUniformWarning)
		{
			// Debug::Warning(name + " :No such uniform name", __FILE__, __LINE__);
			writeBadUniformWarning = false; /// I already told you, don't repeat the message
		}
		return -1;
	}
#endif
	return id->second;
}

void ShaderComponent::SetUniformLocations()
{
	int count;
	GLsizei actualLen;
	GLint size;
	GLenum type;
	char *name;
	int maxUniformListLength;
	unsigned int loc;
	char buffer[512];

	std::string activeUniformList = "Uniforms:\n";
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count);
	sprintf_s(buffer, 512, "There are %d active Uniforms\n", count);

	/// get the length of the longest named uniform
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformListLength);

	/// Create a little buffer to hold the uniform's name - old C memory call just for fun
	name = (char *)malloc(sizeof(char) * maxUniformListLength);

	for (int i = 0; i < count; ++i)
	{

		/// Get the name of the ith uniform

		glGetActiveUniform(shaderID, i, maxUniformListLength, &actualLen, &size, &type, name);
		/// Get the (unsigned int) loc for this uniform
		loc = glGetUniformLocation(shaderID, name);
		std::string uniformName = name;
		uniformMap[uniformName] = loc;

		sprintf_s(buffer, 512, "\"%s\" loc:%d\n", uniformName.c_str(), uniformMap[uniformName]);
		activeUniformList += buffer;
	}
	// Debug::Info(activeUniformList, __FILE__, __LINE__);
	free(name);
}

char *ShaderComponent::ReadTextFile(const char *filename)
{
	char *buffer = nullptr;
	std::ifstream file(filename, std::ios::binary);
	if (file.is_open())
	{										/// Open the file
		file.seekg(0, std::ios::end);		/// goto the end of the file
		int bufferSize = (int)file.tellg(); /// Get the length of the file
		if (bufferSize == 0)
		{ /// If zero, bad file
			std::string errorMsg("Can't read shader file: ");
			std::string str2(filename);
			errorMsg += str2;
			throw errorMsg; /// Bail out
		}
		buffer = new char[(int)(bufferSize + 1)]; /// Need to NULL terminate the array
		file.seekg(0, std::ios::beg);
		file.read(buffer, bufferSize);
		buffer[bufferSize] = NULL; /// Add the NULL
		file.close();
	}
	else
	{
		std::string errorMsg("Can't open shader file: ");
		errorMsg += filename;
		// Debug::Error(errorMsg, __FILE__, __LINE__);
	}
	return buffer;
}

/// Not used, but must be made concrete
void ShaderComponent::Update(const float deltaTime) {};
void ShaderComponent::Render() const {};