#pragma once

/* Класс для физических вычислений */
class PhysicsEngine
{
public:	
	map <int, Planet> planets;
	vector <Explosion>* explosions;
	vector <int> exploiding_id;
	int current_id;
	
	void Initialize();
	void UpdatePosition();
	void CalculateAccelerations();
	void MoveGravity();
	void CollidePlanets();
	void CoursorPlanetOn(Vector2f coursor_position);
	void CoursorPlanetOff();
	void GetOutBorders();
	void GeneratePlanets();
	void Merge(map<int, Planet>::iterator first_planet, map<int, Planet>::iterator second_planet);
	void CheckEndExplosions();
};