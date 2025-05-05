#pragma once

#include "platform.h"
#include "game_object.h"
#include "light.h"
#include "camera.h"
#include <vector>

class Scene 
{
public:
	Camera camera;
	std::vector<GameObject> gameObjects;
	std::vector<Light> lights;
};