#pragma once

class Explosion
{
public:
	Explosion(int m_step, map<int, Planet>::iterator m_i, map<int, Planet>::iterator m_j);
	Explosion() = default;

	Sprite image;
	Vector2f place;
	int step;
	map<int, Planet>::iterator i;
	map<int, Planet>::iterator j;

	void PrepareDrawing(Texture & expl_texture);
};