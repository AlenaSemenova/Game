#include "Header.h"

#define drawing_step 40
#define calculation_step 20
#define NUM_PLANETS_TEXTURES 2

mutex _mutex;
std::atomic<bool> end_game(false);
float dt;

void InitializeTextures(Texture*  textures);

/*  Нить вычислений */
void CalculationThread(PhysicsEngine* planets)
{
	Clock calculation_clock;
	/* Выполняется, пока процесс в main не установил end_game = 1 */
	while (!end_game)
	{
		_mutex.lock();
		planets->UpdatePosition();
		_mutex.unlock();

		/* Время обновления координат - 20 миллисекунд */
		sleep(milliseconds(calculation_step));
		calculation_clock.restart();
	}	
}

int main()
{
	map <int, Planet> planets_drawing;
	PhysicsEngine planets_calculation;
	Texture textures[NUM_PLANETS_TEXTURES];
	InitializeTextures(textures);

	planets_calculation.Initialize(textures);
	planets_calculation.GetSnapshot(planets_drawing);
	thread Thread(CalculationThread, &planets_calculation);

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Game");
	Clock elapsed_time, drawing_clock;
	Event event;
	Vector2f mouse_position;
	map<int, Planet>::iterator it_my_planet = planets_drawing.begin(), it_coursor = ++(planets_drawing.begin());
	bool mouse_prev_state = 0;
	while (window.isOpen())
	{	
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				end_game = true;
				Thread.join();
				window.close();
			}				
		}

		if (Mouse::isButtonPressed(Mouse::Left) && !mouse_prev_state)
		{
			mouse_position = Vector2f(sf::Mouse::getPosition(window));
			planets_calculation.CoursorPlanetOn(mouse_position);
			(it_coursor->second).SetСharacteristics(mouse_position, it_my_planet->second.radius,
				it_my_planet->second.mass, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
			mouse_prev_state = 1;
		}

		if (!(Mouse::isButtonPressed(Mouse::Left)) && mouse_prev_state)
		{
			planets_calculation.CoursorPlanetOff();
			mouse_prev_state = 0;
		}

		if (mouse_prev_state)
			Mouse::setPosition(Vector2i(mouse_position), window);		

		/* Обновляем расположение планет */
		_mutex.lock();
		planets_calculation.GetSnapshot(planets_drawing);
		dt = elapsed_time.getElapsedTime().asSeconds(); //записываем точное прошедшее время в секундах (нужно для вычислений)
		elapsed_time.restart();
		_mutex.unlock();

		/* Рисуем */
		window.clear();
		for (map<int, Planet>::iterator j = planets_drawing.begin(); j != planets_drawing.end(); ++j)
		{
			window.draw(j->second);
		}			
		window.display();

		/* Шаг отрисовки - 40 миллисекунд */
		sleep(milliseconds(drawing_step));
		drawing_clock.restart();	
	}		
}

void InitializeTextures(Texture* textures)
{
	textures[0].loadFromFile("Gnome-Weather-Clear-64.png");
	textures[1].loadFromFile("Face-Angry-128.png");
}