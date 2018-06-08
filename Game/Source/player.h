#pragma once
#include "s00nya.h"
#include <iostream>

using namespace s00nya;

class Player : public GameObject2D
{

public:
	Input & input;
	Timer& timer;
	Float speed = 200.0f;
	Float lastTime = 0.0f;

	Player() :
		GameObject2D(
			Transform2D({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, 0.0f),
			Dimension(50.0f, 80.0f),
			Material()),
		input(Game2D::GetInput()),
		timer(Game2D::GetTimer())
	{
		this->material.frame = 0;
		this->material.diffuse = "Player";
		this->material.type = Material::Type::CLAMP_TO_EDGE;
	}

	void Update() override
	{
		if (input.Held(Keys::KEY_A))
			transform.position.x -= (timer.DeltaTime()*speed);

		if (input.Held(Keys::KEY_D))
			transform.position.x += (timer.DeltaTime()*speed);

		if (input.Held(Keys::KEY_W))
			transform.position.y += (timer.DeltaTime()*speed);

		if (input.Held(Keys::KEY_S))
			transform.position.y -= (timer.DeltaTime()*speed);
	}

};