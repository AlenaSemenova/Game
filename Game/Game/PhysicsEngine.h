#pragma once

class PhysicsEngine
{
public:	
	map <int, Planet> planets;
	
	void Initialize(Texture* textures);
	void UpdatePosition();
	void CalculateAccelerations();
	void GravityMovement();
	void Collision();
	void GetSnapshot(map<int, Planet>& copy_planets);
	void CoursorPlanetOn(Vector2f coursor_position);
	void CoursorPlanetOff();
	void GetOutBorders();
};