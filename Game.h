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
	float cameraSpeedX = 1.0f;
	float cameraSpeedZ = 1.0f;
	float cameraSpeedModifier = 1.1f;
	float renderDistance = 5.0f;

	std::chrono::time_point<std::chrono::system_clock> lastFrame;

	CorridorSegment corridorBase;

	Game();
	~Game();
	void NextFrame();
	void Render();
};

