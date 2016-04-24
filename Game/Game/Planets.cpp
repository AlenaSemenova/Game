#include "Header.h"

void Planet::SetСharacteristics(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc)
{
	setPosition(m_pos);
	radius = m_radius;
	mass = m_mass;
	velocity = m_vel;
	acceleration = m_acc;
}