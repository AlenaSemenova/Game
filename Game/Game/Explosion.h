#pragma once

class Explosion
{
public:
	Explosion(int m_step, map<int, Planet>::iterator _first_planet, map<int, Planet>::iterator _second_planet);
	Explosion() = default;

	Sprite image;
	Vector2f place;
	int step;
	map<int, Planet>::iterator first_planet;
	map<int, Planet>::iterator second_planet;

	void PrepareDrawing(Texture & expl_texture);
};