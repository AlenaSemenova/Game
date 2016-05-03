#include "Header.h"

#define gravity 500000
//минимальное расстояние между планетами, для которого имеет смысл считать ускорение//
#define r_min 50 
#define my_planet_radius 10.0f
#define my_planet_mass 10.0f

extern float dt;
extern mutex _mutex;

/* Здесь надо будет подумать насчет масштабирования и способа хранения текстур */
void PhysicsEngine::Initialize(Texture*  textures)
{
	/* Создает нашу планету (в центре экрана, масса = 1, скорость и ускорение = 0) и добавляет ее в planets[0] */
	Vector2f pos_my_planet = Vector2f(float(WINDOW_WIDTH / 2), float(WINDOW_HEIGTH / 2));
	Planet *p_my_planet = new Planet(pos_my_planet, my_planet_radius, my_planet_mass, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), &(textures[0]));
	planets.insert(pair <int, Planet> (0, *p_my_planet));

	/*  В planets[1] всегда будет храниться наша псевдопланета.
	Если мышка не нажата - обнуляем массу, если нажата - присваиваем массу нашей планеты */
	Planet *p_cursor = new Planet(Vector2f(0.0f, 0.0f), 0, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), &(textures[1]));
	p_cursor->setColor(Color(0, 0, 0, 0));
	planets.insert(pair <int, Planet>(1, *p_cursor));	
}

void PhysicsEngine::UpdatePosition()
{
	GetOutBorders();
	CalculateAccelerations();
	GravityMovement();

	/* Скорость и ускорения курсора всегда равны нулю */
	map<int, Planet>::iterator coursor = ++(planets.begin());
	coursor->second.acceleration = Vector2f(0.0f, 0.0f);
	coursor->second.velocity = Vector2f(0.0f, 0.0f);
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
			if (r_abs > r_min)
			{
				f_abs = gravity / (r_abs * r_abs);
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
			j->second.SetСharacteristics(i->second.getPosition(), i->second.radius, i->second.mass, i->second.velocity, i->second.acceleration);
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

void PhysicsEngine::CoursorPlanetOn(Vector2f coursor_position)
{
	map<int, Planet>::iterator my_planet = planets.begin(), coursor = ++(planets.begin());
	_mutex.lock();
	coursor->second.SetСharacteristics(coursor_position, my_planet->second.radius, my_planet -> second.mass, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0));
	_mutex.unlock();
}

void PhysicsEngine::CoursorPlanetOff()
{
	map<int, Planet>::iterator my_planet = planets.begin(), coursor = ++(planets.begin());
	_mutex.lock();
	coursor->second.SetСharacteristics(Vector2f(0.0f, 0.0f), 0.0f, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
	_mutex.unlock();
}

inline bool GetOutXBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((WINDOW_WIDTH - (i->second.getPosition().x)) <= i->second.radius || i->second.getPosition().x <= i->second.radius);
}

inline bool GetOutYBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((WINDOW_HEIGTH - (i->second.getPosition().y)) <= i->second.radius || i->second.getPosition().y <= i->second.radius);
}

void PhysicsEngine::GetOutBorders()
{
	map<int, Planet>::iterator i = planets.begin();
	
	if (GetOutYBorders(planets, i))
		i->second.velocity.y = -(i->second.velocity.y);
	if (GetOutXBorders(planets, i))
		i->second.velocity.x = -(i->second.velocity.x);
	
	if (planets.size() <= 2)
		return;
	
	advance(i, 2);
	for (; i != planets.end(); ++i)
	{
		if (GetOutYBorders(planets, i) || GetOutXBorders(planets, i))
			planets.erase(i);
	}
}
