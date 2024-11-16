#ifndef BASESCENE_H
#define BASESCENE_H
#include "Scene.h"
#include "SimpleShape.h"
#include <Matrix.h>
union SDL_Event;
class Window;
#include "Camera.h"

#include "Bridge.h"


class BaseScene : public Scene {
private:
	
	bool drawInWireMode = false;
	bool IsPaused;
	Camera cam;

	float dt;

	Matrix4 modelMatrix;
	SDL_Renderer* screenRenderer;
	SimpleShape shapeRenderer;

	Bridge bridge; 

public:
	explicit BaseScene();
	virtual ~BaseScene();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	void DubugGUI();

	
};


#endif // BASESCENE_H