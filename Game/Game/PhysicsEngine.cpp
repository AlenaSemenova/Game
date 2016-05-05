#include "Header.h"

#define gravity 1000
//минимальное расстояние между планетами, для которого имеет смысл считать ускорение//
#define r_min 50 
#define my_planet_radius 10.0f
#define my_planet_mass 10.0f
#define NUM_PLANETS 5
//используется при генерации планет//
#define block_size 50
#define radius_dispersion 5
#define mass_dispersion 5
#define velocity_dispersion 5

extern float dt;
extern mutex _mutex;
int current_id = 2;

bool is_occupied_place[WINDOW_WIDTH / block_size][WINDOW_HEIGTH / block_size];
extern Texture textures[NUM_PLANETS_TEXTURES];

/* Здесь надо будет подумать насчет масштабирования и способа хранения текстур */
void PhysicsEngine::Initialize()
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
	PlanetsGeneration();
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
				r_abs = r_min;
			f_abs = gravity / (r_abs * r_abs);
			i->second.acceleration = ((f_abs *  j->second.mass) / r_abs) * r;
			j->second.acceleration = -((f_abs *  i->second.mass) / r_abs) * r;
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
	map<int, Planet>::iterator i = planets.begin(), j = copy_planets.begin(), tmp;
	for (; i != planets.end() && j != copy_planets.end(); ++i)
	{
		while (i != planets.end() && j != copy_planets.end() && i->first > j->first)
		{
			tmp = j++;
			copy_planets.erase(tmp);
		}
		if (j != copy_planets.end() && i->first == j->first)
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
		tmp = j++;
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
	map<int, Planet>::iterator tmp;
	while (i != planets.end())
	{
		if (GetOutYBorders(planets, i) || GetOutXBorders(planets, i))
		{
			tmp = i++;
			planets.erase(tmp);
		}
		else
			++i;
	}
}

bool FindEmptyPlace(map<int, Planet>& planets, Vector2f& empty_place)
{
	Vector2i pos_corner_view = Vector2i(planets[0].getPosition()) - Vector2i(VIEW_WIDTH / 2, VIEW_HEIGTH / 2);
	if (pos_corner_view.x < 0 || pos_corner_view.y < 0)
		pos_corner_view = Vector2i(0, 0);

	int tmp_x = (pos_corner_view.x + VIEW_WIDTH) / block_size, tmp_y = (pos_corner_view.y + VIEW_HEIGTH) / block_size;
	for (int i = pos_corner_view.x / block_size; i <= tmp_x; ++i)
	{
		for (int j = pos_corner_view.y / block_size; j <= tmp_y; ++j)
			is_occupied_place[i][j] = true;
	}

	Vector2i planet_pos;
	for (map<int, Planet>::iterator it = planets.begin(); it != planets.end(); ++it)
	{
		planet_pos = Vector2i(it->second.getPosition());
		is_occupied_place[planet_pos.x / block_size][planet_pos.y / block_size] = true;
	}
	
	int N = (WINDOW_WIDTH / block_size), size = (WINDOW_WIDTH / block_size) * (WINDOW_HEIGTH / block_size);
	int random_i = rand() % size;
	Vector2f return_pos;
	for (int i = random_i; i < size; ++i)
	{
		if (!is_occupied_place[i / N][i % N])
		{
			empty_place = Vector2f(((i / N) * block_size) + block_size / 2, ((i % N) * block_size) + block_size / 2);
			return true;
		}
	}
	for (int i = 0; i < random_i; ++i)
	{
		if (!is_occupied_place[i / N][i % N])
		{
			empty_place = Vector2f(((i / N) * block_size) + block_size / 2, ((i % N) * block_size) + block_size / 2);
			return true;
		}
	}
	return false;
}

void PhysicsEngine::PlanetsGeneration()
{ 
	Vector2f pos_new_planet;
	Planet *p_new_planet;
	Vector2f pos_centre_view = planets[0].getPosition(), r;
	Vector2f new_planet_vel;
	while (planets.size() < NUM_PLANETS)
	{
		if (!(FindEmptyPlace(planets, pos_new_planet)))
			break;
		r = pos_new_planet - pos_centre_view;
		new_planet_vel = ((rand() % velocity_dispersion) / Length(r)) * r;
		p_new_planet = new Planet(pos_new_planet, planets[0].radius + rand() % radius_dispersion, planets[0].mass + rand() % mass_dispersion, 
			new_planet_vel, Vector2f(0.0f, 0.0f), &(textures[1 + rand() % (NUM_PLANETS_TEXTURES - 1)]));
		planets.insert(pair <int, Planet>(current_id, *p_new_planet));
		++current_id;
	}
}
