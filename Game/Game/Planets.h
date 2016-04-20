#pragma once

class Planet: public Sprite
{
public:
	/* Конструктор: устанавливает расположение, масштабирование(!), массу, скорость, ускорение, текстуру */
	Planet(Vector2f m_pos, Vector2f m_scale, float m_mass, Vector2f m_vel, Vector2f m_acc, Texture m_texture) : Sprite(m_texture)
	{
		setPosition(m_pos);
		setScale(m_scale);
		mass = m_mass;
		velocity = m_vel;
		acceleration = m_acc;
	}
	float mass;
	Vector2f velocity;
	Vector2f acceleration;
};
