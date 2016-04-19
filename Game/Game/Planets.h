#pragma once
#include <SFML\Graphics.hpp>
using namespace std;
using namespace sf;

class Planets: public Sprite
{
public:
	/* Конструктор: устанавливает расположение, масштабирование(!), массу, скорость, ускорение, текстуру */
	Planets(Vector2f m_pos, Vector2f m_scale, float m_mass, float m_vel, float m_acc, Texture m_texture) : Sprite(m_texture)
	{
		setPosition(m_pos);
		setScale(m_scale);
		mass = m_mass;
		velocity = m_vel;
		acceleration = m_acc;
	}
	float mass;
	float velocity;
	float acceleration;
};