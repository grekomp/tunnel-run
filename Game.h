#pragma once
#include "GameStatus.h"
#include <vector>
#include "Segment.h"
#include "CorridorSegment.h"
#include <chrono>

class Game
{
public:
	GameStatus status;
	std::vector<Segment> segments;
	std::vector<CorridorSegment> corridorSegments;
	float cameraSpeedX = 0.1f;
	float cameraSpeedZ = 10.0f;
	float cameraSpeedModifier = 1.1f;
	float renderDistance = 50.0f;
	Model ball;
	float ballOffset = 2.5f;
	std::chrono::time_point<std::chrono::system_clock> lastFrame;
	glm::mat4 viewMatrix;

	CorridorSegment corridorBase;
	Obstacle obstacleBase;
	Pickup pickupBase;
	std::vector<Segment> baseSegments;

	Game();
	~Game();
	void FirstFrame();
	void NextFrame();
	void Render();
	void RenderPickups();
};

