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
	planets.insert(pair <int, Planet> (0, *p_my_planet));

	/*  В planets[1] всегда будет храниться наша псевдопланета.
	Если мышка не нажата - обнуляем массу, если нажата - присваиваем массу нашей планеты */
	Texture *texture_cursor = new Texture();
	(*texture_cursor).loadFromFile("Face-Angry-128.png");
	Planet *p_cursor = new Planet(vect_0_0, vect_1_1, 0.0f, vect_0_0, vect_0_0, *texture_cursor);
	planets.insert(pair <int, Planet>(1, *p_cursor));
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

	for (map<int,Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		for (map<int, Planet>::iterator j = planets.begin(); j != i; ++j)
		{
			r = j->second.getPosition() - i->second.getPosition();
			r_abs = Length(r);
			f_abs = gravity / (r_abs * r_abs);
			if (r_abs > r_min)
			{
				i->second.acceleration = ((f_abs *  j->second.mass) / r_abs) * r;
				j->second.acceleration = -((f_abs *  i->second.mass) / r_abs) * r;
			}
		}
	}
}

/* Изменяет положение планет, основываясь только на их гравитационном взаимодействии (без столкновений)*/
void PhysicsEngine::GravityMovement()
{
	Vector2f dv;
	int size = planets.size();
	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		dv = i->second.acceleration * dt;
		i->second.setPosition(i->second.getPosition() + (i->second.velocity + dv / 2.0f) * dt);
		i->second.velocity += dv;
	}
}

void PhysicsEngine::Collision()
{
	Vector2f r;
	int size = planets.size();
	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		for (map<int, Planet>::iterator j = planets.begin(); j != i; ++j)
		{
			r = j->second.getPosition() - i->second.getPosition();
		}
	}
}

void PhysicsEngine::UpdatePosition()
{
	CalculateAccelerations();
	GravityMovement();
}

void PhysicsEngine::GetSnapshot(map<int, Planet>& copy_planets)
{
	int tmp;
	map<int, Planet>::iterator i = planets.begin(), j = copy_planets.begin();
	for (; i != planets.end() && j != copy_planets.end(); ++i)
	{
		while (i != planets.end() && j != copy_planets.end() && i->first > j->first)
		{
			tmp = j->first;
			++j;
			copy_planets.erase(tmp);
		}
		if (i->first == j->first)
		{
			j->second.setPosition(i->second.getPosition());
			j->second.setScale(i->second.getScale());
			j->second.mass = i->second.mass;
			j->second.velocity = i->second.velocity;
			j->second.acceleration = i->second.acceleration;
			++j;
		}
	}
	while (i != planets.end())
	{
		copy_planets.insert(*i);
		++i;
	}
	while (j != copy_planets.end())
	{
		tmp = j->first;
		++j;
		copy_planets.erase(tmp);
	}		
}