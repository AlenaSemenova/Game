#pragma once

class Planet
{
public:
	/* Конструктор: устанавливает расположение, радиус, массу, скорость, ускорение, текстуру */
	Planet(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc, int num_image);
	Planet() = default;

	Vector2f position;
	float radius;
	float mass;
	Vector2f velocity;
	Vector2f acceleration;
	Sprite* image;

	void SetСharacteristics(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc);
	Vector2f getPosition();
	void setPosition(Vector2f new_pos);
	void GetInfo();
};
