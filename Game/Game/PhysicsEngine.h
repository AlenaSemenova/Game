#pragma once

class PhysicsEngine
{
public:	
	map<int, Planet> planets;
	
	void Initialize();
	void UpdatePosition();
	void CalculateAccelerations();
	void GravityMovement();
	void Collision();
	void GetSnapshot(map<int, Planet>& copy_planets);
};