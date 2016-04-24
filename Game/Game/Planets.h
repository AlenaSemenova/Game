#pragma once

class Planet: public Sprite
{
public:
	/* Конструктор: устанавливает расположение, радиус, массу, скорость, ускорение, текстуру */
	Planet(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc, Texture m_texture) : Sprite(m_texture)
	{
		setPosition(m_pos);
		radius = m_radius;
		mass = m_mass;
		velocity = m_vel;
		acceleration = m_acc;
	}
	Planet() = default;
	float radius;
	float mass;
	Vector2f velocity;
	Vector2f acceleration;
	void SetСharacteristics(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc);
};
