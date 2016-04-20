#pragma once

class PhysicsEngine
{
public:
	vector<Planet> planets;

	void Initialize();
	void UpdatePosition();
	void CalculateAccelerations();
	void GravityMovement();
	void Collision();
};