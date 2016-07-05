#include "Header.h"

extern Texture planets_textures[NUM_PLANETS_TEXTURES];

Planet::Planet(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc, int num_image)
{
	position = m_pos;
	radius = m_radius;
	mass = m_mass;
	velocity = m_vel;
	acceleration = m_acc;
	image = new Sprite(planets_textures[num_image]);
}

void Planet::SetСharacteristics(Vector2f m_pos, float m_radius, float m_mass, Vector2f m_vel, Vector2f m_acc)
{
	position = m_pos;
	radius = m_radius;
	mass = m_mass;
	velocity = m_vel;
	acceleration = m_acc;
}

Vector2f Planet::getPosition()
{
	return position;
}

void Planet::setPosition(Vector2f new_pos)
{
	position = new_pos;
}

void Planet::GetInfo()
{
	cout << "position: " << position.x << " " << position.y << " radius: " << radius << " mass: " << mass << " velocity: "
		<< velocity.x << " " << velocity.y << " acceleration: " << acceleration.x << " " << acceleration.y << endl;
}
