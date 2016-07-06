#include "Header.h"

#define gravity 10
//минимальное расстояние между планетами, для которого имеет смысл считать ускорение//
#define r_min 50 
//используется при создании нашей планеты//
#define my_planet_radius 35.0f
#define my_planet_mass 10.0f
//Число планет в окне (не во view) = const //
#define NUM_PLANETS 5
//используется при генерации планет//
#define block_size 50
#define radius_dispersion 25
#define radius_dispersion_min 25
#define velocity_dispersion 5
#define mass_coeff 3
#define end_explosion_step 10

extern float dt;
extern mutex _mutex;
extern mutex expl_mutex;
extern std::atomic<bool> end_game;

void PhysicsEngine::Initialize()
{
	/* Создает нашу планету (в центре экрана, масса = 1, скорость и ускорение = 0) и добавляет ее в planets[0] */
	Vector2f pos_my_planet = Vector2f(float(WINDOW_WIDTH / 2), float(WINDOW_HEIGTH / 2));
	planets.emplace(0, Planet(pos_my_planet, my_planet_radius, my_planet_mass, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), 0));

	/*  В planets[1] всегда будет храниться наша псевдопланета.
	Если мышка не нажата - обнуляем массу, если нажата - присваиваем массу нашей планеты */
	planets.emplace(1, Planet(Vector2f(0.0f, 0.0f), 0, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), 1));
	planets[1].image -> setColor(Color(0, 0, 0, 0));

	current_id = 2;
}

void PhysicsEngine::UpdatePosition()
{
	GetOutBorders();
	GeneratePlanets();
	CalculateAccelerations();
	MoveGravity();
	CollidePlanets();
	CheckEndExplosions();
}

/*длина вектора r*/
inline float Length(Vector2f r)
{
	return sqrt((r.x * r.x) + (r.y * r.y));
}

/* Рассчитывает ускорения планет */
void PhysicsEngine::CalculateAccelerations()
{
	Vector2f r;
	float r_abs, f_abs;

	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
		i->second.acceleration = Vector2f(0,0);
	
	map<int, Planet>::iterator it_coursor = ++planets.begin();
	for (map<int,Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		for (map<int, Planet>::iterator j = planets.begin(); j != i; ++j)
		{
			if (j == it_coursor)
				continue;
			r = j->second.getPosition() - i->second.getPosition();
			r_abs = Length(r);
			if (r_abs > r_min)
				r_abs = r_min;
			f_abs = gravity / (r_abs * r_abs);
			i->second.acceleration += ((f_abs *  j->second.mass) / r_abs) * r;
			j->second.acceleration += -((f_abs *  i->second.mass) / r_abs) * r;
		}
	}
	
	planets[1].acceleration = Vector2f(0, 0);
}

/* Изменяет положение планет, основываясь только на их гравитационном взаимодействии (без столкновений)*/
void PhysicsEngine::MoveGravity()
{
	Vector2f dv;
	int size = planets.size();
	map<int, Planet>::iterator it_coursor = ++(planets.begin());
	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		if (i == it_coursor)
			continue;
		dv = i->second.acceleration * dt;
		i->second.setPosition(i->second.getPosition() + (i->second.velocity + dv / 2.0f) * dt);
		i->second.velocity += dv;
	}
}

/* Обрабатывает столкновения планет */
void PhysicsEngine::CollidePlanets()
{
	Vector2f r, place;
	int size = planets.size();
	float distance;
	map<int, Planet>::iterator it_cursor = ++(planets.begin());
	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		for (map<int, Planet>::iterator j = planets.begin(); j != i; ++j)
		{
			r = j->second.getPosition() - i->second.getPosition();
			distance = Length(r);
			if (distance < j->second.radius + i->second.radius)
			{
				if ((i == planets.begin() && i->second.radius < j->second.radius)
					|| (j == planets.begin() && j->second.radius < i->second.radius))
					end_game = true;
					
				else if ((i == it_cursor) || (j == it_cursor))
					continue;
				else
				{ 
					int k = 0;
					while (k < exploiding_id.size() && exploiding_id[k] != i->first && exploiding_id[k] != j->first)
						++k;

					if (k != exploiding_id.size())
						continue;
					
					place = i->second.getPosition() + r / 2.0f;
					expl_mutex.lock();
					explosions->emplace_back(Explosion(0, i, j));
					expl_mutex.unlock();
					
					exploiding_id.push_back(i->first);
				}			
			}
		}
	}
}

void PhysicsEngine::CheckEndExplosions()
{
	if (explosions->empty())
		return;
	
	expl_mutex.lock();
	vector<int>::iterator k;
	for (vector<Explosion>::iterator it = explosions->begin(); it != explosions->end(); )
	{
		if (it->step == end_explosion_step / 2)
		{
			k = exploiding_id.begin();
			while (*k != it->first_planet->first && *k != it->second_planet->first)
				++k;
			exploiding_id.erase(k);

			Merge(it->first_planet, it->second_planet);
			it = explosions->erase(it);
		}
		else
			++it;
	}
	expl_mutex.unlock();
}

void PhysicsEngine::Merge(map<int, Planet>::iterator first_planet, map<int, Planet>::iterator second_planet)
{
	float new_mass;
	if (second_planet->second.mass == first_planet->second.mass)
	{
		planets.erase(first_planet);
		planets.erase(second_planet);
		return;
	}

	Planet planet1 = first_planet->second, planet2 = second_planet->second, planet_res;
	new_mass = planet1.mass + planet2.mass;
	planet_res.velocity = (planet1.mass * planet1.velocity + planet2.mass *  planet2.velocity) / new_mass;
	planet_res.mass = new_mass;

	if (planet1.mass > planet2.mass)
	{
		planet_res.radius = planet1.radius * pow(new_mass / planet1.mass, 1.0f / 3.0f);
		first_planet->second.SetСharacteristics(first_planet->second.position, planet_res.radius, planet_res.mass, planet_res.velocity, first_planet->second.acceleration);
		planets.erase(second_planet);
	}	
	else
	{
		planet_res.radius = planet2.radius * pow(new_mass / planet2.mass, 1.0f /3.0f);
		second_planet->second.SetСharacteristics(second_planet->second.position, planet_res.radius, planet_res.mass, planet_res.velocity, second_planet->second.acceleration);
		planets.erase(first_planet);
	}		
}

/* Задает курсору массу, равную массе всех существующих планет, во время нажатия мышки */
void PhysicsEngine::CoursorPlanetOn(Vector2f coursor_position)
{
	float sum_mass = 0;
	
	if (planets.size() <= 2)
		return;
	map<int, Planet>::iterator i = planets.begin();
	advance(i, 2);
	
	for (; i != planets.end(); ++i)
		sum_mass += i->second.mass;
	sum_mass *= mass_coeff;

	_mutex.lock();
	planets[1].SetСharacteristics(coursor_position, planets[0].radius, sum_mass * mass_coeff, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0));
	_mutex.unlock();
}

/* Обнуляет массу куросора, при отпускании мышки */
void PhysicsEngine::CoursorPlanetOff()
{
	map<int, Planet>::iterator my_planet = planets.begin(), coursor = ++(planets.begin());
	_mutex.lock();
	coursor->second.SetСharacteristics(Vector2f(0.0f, 0.0f), 0.0f, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
	_mutex.unlock();
}

inline bool OnXBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((WINDOW_WIDTH - (i->second.getPosition().x)) <= i->second.radius || i->second.getPosition().x <= i->second.radius);
}

inline bool OnYBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((WINDOW_HEIGTH - (i->second.getPosition().y)) <= i->second.radius || i->second.getPosition().y <= i->second.radius);
}

inline bool GetOutXBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((-WINDOW_WIDTH + (i->second.getPosition().x)) >= i->second.radius || i->second.getPosition().x <= -(i->second.radius));
}

inline bool GetOutYBorders(map<int, Planet>& planets, map<int, Planet>::iterator i)
{
	return ((-WINDOW_HEIGTH + (i->second.getPosition().y)) >= i->second.radius || i->second.getPosition().y <= -(i->second.radius));
}

/* При столкновении с границами окна:
	наша планета  упруго отражается, другие планеты удаляются из списка */
void PhysicsEngine::GetOutBorders()
{
	map<int, Planet>::iterator i = planets.begin();
	
	if (OnYBorders(planets, i))
		i->second.velocity.y = -(i->second.velocity.y);
	if (OnXBorders(planets, i))
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

Vector2f GetCornerView(Planet & planet)
{
	Vector2f view_pos;
	if (planet.getPosition().y < VIEW_HEIGTH / 2)
		view_pos.y = VIEW_HEIGTH / 2;
	else if (planet.getPosition().y > WINDOW_HEIGTH - (VIEW_HEIGTH / 2))
		view_pos.y = WINDOW_HEIGTH - (VIEW_HEIGTH / 2);
	else
		view_pos.y = planet.getPosition().y;

	if (planet.getPosition().x < VIEW_WIDTH / 2)
		view_pos.x = VIEW_WIDTH / 2;
	else if (planet.getPosition().x > WINDOW_WIDTH - (VIEW_WIDTH / 2))
		view_pos.x = WINDOW_WIDTH - (VIEW_WIDTH / 2);
	else
		view_pos.x = planet.getPosition().x;

	return (view_pos - Vector2f(VIEW_WIDTH / 2, VIEW_HEIGTH / 2));
}

bool is_occupied_place[WINDOW_WIDTH / block_size][WINDOW_HEIGTH / block_size];

/* Находит место для новой планеты. Для этого используется глобальный массив is_occupied_place, 
	который представляет из себя игровое поле, разбитое на сектора размера block_size * block_size.
	Сектор помечается занятым, если в нем находится другая планета или он в видимости пользователя.
	Из пустых клеток рандомно выбирается любая. Функция возвращает значение bool, найдено ли место, и передает в empty_place его координаты*/
bool FindEmptyPlace(map<int, Planet>& planets, Vector2f& empty_place)
{
	int N = (WINDOW_WIDTH / block_size), M = (WINDOW_HEIGTH / block_size);
	int size = N * M;

	/* Обнуляем массив is_occupied_place*/
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < M; ++j)
			is_occupied_place[i][j] = false;
	}

	Vector2f pos_corner_view = GetCornerView(planets[0]);

	/* Отмечает блоки, занятые view (то есть в видимости пользователя) */
	int tmp_x = (pos_corner_view.x + VIEW_WIDTH) / block_size, tmp_y = (pos_corner_view.y + VIEW_HEIGTH) / block_size;
	for (int i = pos_corner_view.x / block_size; i <= tmp_x; ++i)
	{
		for (int j = pos_corner_view.y / block_size; j <= tmp_y; ++j)
			is_occupied_place[i][j] = true;
	}

	/* Отмечает блоки, зантяые другими планетами */
	Vector2i planet_pos;
	map<int, Planet>::iterator it_coursor = ++planets.begin();
	for (map<int, Planet>::iterator it = planets.begin(); it != planets.end(); ++it)
	{
		if (it == it_coursor)
			continue;
		planet_pos = Vector2i(it->second.getPosition());
		is_occupied_place[planet_pos.x / block_size][planet_pos.y / block_size] = true;
	}
	
	/* Выбираем любой из свобоных блоков */
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

/* Количество планет поддерживается (почти) константой. 
 Если на этом шаге мы не нашли пустое место и FindEmptyPlace вернуло false, то откладываем добавление планет до следующего шага.*/
void PhysicsEngine::GeneratePlanets()
{ 
	Vector2f pos_new_planet;
	Vector2f pos_centre_view = planets[0].getPosition(), r;
	Vector2f new_planet_vel;
	float new_planet_mass, new_planet_radius, density = planets[0].mass / pow(planets[0].radius, 3);
	while (planets.size() < NUM_PLANETS)
	{
		if (!(FindEmptyPlace(planets, pos_new_planet)))
			break;
		r = pos_new_planet - pos_centre_view;
		new_planet_vel = Vector2f(0, 0);
		new_planet_radius = planets[0].radius * (1.0f +  float((pow(-1, rand() % 2) * (radius_dispersion_min + rand() % radius_dispersion))) / 100.0f);
		new_planet_mass = density * pow(new_planet_radius, 3);
		planets.emplace(current_id, Planet(pos_new_planet, new_planet_radius, new_planet_mass,
			new_planet_vel, Vector2f(0.0f, 0.0f), 2 + rand() % (NUM_PLANETS_TEXTURES - 2)));
		++current_id;
	}
}