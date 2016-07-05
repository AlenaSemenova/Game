#include "Header.h"

#define drawing_step 40
#define calculation_step 20

mutex _mutex; //при обращении к PhysicsEngine любая из нитей блокирует _mutex
mutex expl_mutex; //при обращении к вектору explosions нити блокируют expl_mutex
std::atomic<bool> window_closed(false);
std::atomic<bool> end_game(false);
float dt;
Texture planets_textures[NUM_PLANETS_TEXTURES];

void DisplayMenu(Scene & window);
void InitializePlanetsTextures(Texture* planets_textures);
void InitializeSkyTextures(Texture* sky_textures);
void GameOver(std::thread & thread, Scene & window);

/*  Нить вычислений */
void CalculationThread(PhysicsEngine* planets_calculation)
{
	Clock calculation_time;
	Time sleep_time;
	/* Выполняется, пока процесс в main не установил end_game = 1 */
	while (!window_closed && !end_game)
	{
		_mutex.lock();
		planets_calculation->UpdatePosition();
		_mutex.unlock();

		/* Время обновления координат - 20 миллисекунд */
		sleep_time = milliseconds(calculation_step) - calculation_time.getElapsedTime();
		sleep(milliseconds(calculation_step) - calculation_time.getElapsedTime());
		calculation_time.restart();
	}	
}

int main()
{
	Scene window_drawing(VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Game");
	DisplayMenu(window_drawing);

	PhysicsEngine planets_calculation;
	Texture sky_textures[NUM_SKY_TEXTURES];

	InitializePlanetsTextures(planets_textures);
	InitializeSkyTextures(sky_textures);
	planets_calculation.Initialize();
	window_drawing.Initialize(sky_textures);
	window_drawing.GetSnapshot(planets_calculation);
	planets_calculation.explosions = &(window_drawing.explosions);	

	thread Thread(CalculationThread, &planets_calculation);

	Clock elapsed_time, drawing_time, time_result;
	Event event;
	Vector2f mouse_position;
	map<int, Planet>::iterator it_my_planet = window_drawing.planets.begin(), it_coursor = ++(window_drawing.planets.begin());
	bool mouse_prev_state = 0;
	while (window_drawing.isOpen())
	{			
		while (window_drawing.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window_closed = true;
				GameOver(Thread, window_drawing);
			}				
		}

		if (end_game)
			GameOver(Thread, window_drawing);

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			mouse_position = Vector2f(sf::Mouse::getPosition(window_drawing));
			if (!mouse_prev_state)
				mouse_prev_state = 1;
			planets_calculation.CoursorPlanetOn(mouse_position);
		}

		if (!(Mouse::isButtonPressed(Mouse::Left)) && mouse_prev_state)
		{
			planets_calculation.CoursorPlanetOff();
			mouse_prev_state = 0;
		}

		/* Обновляем расположение планет */
		_mutex.lock();
		window_drawing.GetSnapshot(planets_calculation);
		dt = elapsed_time.getElapsedTime().asSeconds(); //записываем точное прошедшее время в секундах (для вычислений)
		elapsed_time.restart();
		_mutex.unlock();

		/* Рисуем */
		window_drawing.result_time = time_result.getElapsedTime();
		window_drawing.Draw();
		window_drawing.setView(window_drawing.view);
		window_drawing.display();

		/* Шаг отрисовки - 40 миллисекунд */
		sleep(milliseconds(drawing_step) - drawing_time.getElapsedTime());			
		drawing_time.restart();

	}		
}

void  InitializePlanetsTextures(Texture* planets_textures)
{
	planets_textures[0].loadFromFile("images/Network_128.png");
	planets_textures[1].loadFromFile("images/Network_128.png");
	planets_textures[2].loadFromFile("images/GLBASIC_Planet1.png");
	planets_textures[3].loadFromFile("images/GLBASIC_Planet3.png");
	planets_textures[4].loadFromFile("images/greenplanet.png");
	planets_textures[5].loadFromFile("images/planet.png");
	planets_textures[6].loadFromFile("images/planet_x___ice_by_keizgon-d4x69zq.png");
	planets_textures[7].loadFromFile("images/photo.png");
	planets_textures[8].loadFromFile("images/planet-brown_512x512.png");
}

void InitializeSkyTextures(Texture* sky_textures)
{
	
	sky_textures[0].loadFromFile("images/maxresdefault.png");
	sky_textures[1].loadFromFile("images/14532890251crackpot-space-3.png");
	sky_textures[2].loadFromFile("images/asteroid-flying-in-space-wallpaper-1.png");
	sky_textures[3].loadFromFile("images/8PqXCbLu2m-1920x1080.png");
	sky_textures[4].loadFromFile("images/2250835.png");
}

void GameOver(thread & thread, Scene & window)
{
	thread.join();
	window.DrawGameOver();
	window.close();
}