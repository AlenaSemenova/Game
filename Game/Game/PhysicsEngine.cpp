#include "Header.h"

#define gravity 10
//минимальное расстояние между планетами, для которого имеет смысл считать ускорение//
#define r_min 1 
//время для расчетов//
#define dt 0.1f

/* Здесь надо будет подумать насчет масштабирования и способа хранения текстур */
void PhysicsEngine::Initialize()
{
	Vector2f vect_1_1 = Vector2f(1.0f, 1.0f);
	Vector2f vect_0_0 = Vector2f(1.0f, 1.0f);

	/* Создает нашу планету (в центре экрана, масса = 1, скорость и ускорение = 0) и добавляет ее в planets[0] */
	Texture *texture_my_planet = new Texture();
	(*texture_my_planet).loadFromFile("Gnome-Weather-Clear-64.png");
	Vector2f pos_my_planet = Vector2f(float(WINDOW_WIDTH / 2), float(WINDOW_HEIGTH / 2));
	Planet *p_my_planet = new Planet(pos_my_planet, vect_1_1, 1.0f, vect_0_0, vect_0_0, *texture_my_planet);
	planets.push_back(*p_my_planet);

	/*  В planets[1] всегда будет храниться наша псевдопланета.
	Если мышка не нажата - обнуляем массу, если нажата - присваиваем массу нашей планеты */
	Texture *texture_cursor = new Texture();
	(*texture_cursor).loadFromFile("Face-Angry-128.png");
	Planet *p_cursor = new Planet(vect_0_0, vect_1_1, 0.0f, vect_0_0, vect_0_0, *texture_cursor);
	planets.push_back(*p_cursor);
}

/*длина вектора r*/
inline float Length(Vector2f r)
{
	return sqrt((r.x * r.x) + (r.y * r.y));
}

/* Рассчитывает ускорения планет*/
void PhysicsEngine::CalculateAccelerations()
{
	Vector2f r;
	float r_abs, f_abs;

	int size = planets.size();
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			r = planets[j].getPosition() - planets[i].getPosition();
			r_abs = Length(r);
			f_abs = gravity / (r_abs * r_abs);
			if (r_abs > r_min)
			{
				planets[i].acceleration = ((f_abs *  planets[j].mass) / r_abs) * r;
				planets[j].acceleration = -((f_abs *  planets[i].mass) / r_abs) * r;
			}
		}
	}
}

/* Изменяет положение планет, основываясь только на их гравитационном взаимодействии (без столкновений)*/
void PhysicsEngine::GravityMovement()
{
	Vector2f dv;
	int size = planets.size();
	for (int i = 0; i < size; ++i)
	{
		dv = planets[i].acceleration * dt;
		planets[i].setPosition(planets[i].getPosition() + (planets[i].velocity + dv / 2.0f) * dt);
		planets[i].velocity += dv;
	}
}

void PhysicsEngine::Collision()
{
	Vector2f r;
	int size = planets.size();
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			r = planets[j].getPosition() - planets[i].getPosition();
		}
	}
}

void PhysicsEngine::UpdatePosition()
{
	CalculateAccelerations();
	GravityMovement();
}