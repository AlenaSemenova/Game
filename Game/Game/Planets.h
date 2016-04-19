#pragma once
#include <SFML\Graphics.hpp>
using namespace std;
using namespace sf;

class Planets
{
public:
	Planets();
	Vector2f position;
	float radius;
	float mass;
	float velocity;
	float acceleration;
};