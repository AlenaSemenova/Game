#include "Header.h"

#define drawing_step 40
#define calculation_step 20

mutex _mutex;
float dt;

/*  Нить вычислений.
Надо бы подумать, когда она закнчивает работу */
void CalculationThread(PhysicsEngine & planets)
{
	Clock calculation_clock;
	while(1)
	{
		/* Время обновления координат - 20 миллисекунд */
		if (calculation_clock.getElapsedTime().asMilliseconds() > calculation_step)
		{
			_mutex.lock();
			planets.UpdatePosition();
			_mutex.unlock();
		}
	}
		
}

int main()
{
	map <int, Planet> planets_drawing;
	PhysicsEngine planets_calculation;
	planets_calculation.Initialize();
	planets_calculation.GetSnapshot(planets_drawing);
	thread Thread(CalculationThread, planets_calculation);

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Game");
	Clock elapsed_time, drawing_clock;
	Event event;
	while (window.isOpen())
	{
		/* Шаг отрисовки - 40 миллисекунд */
		if (drawing_clock.getElapsedTime().asMilliseconds() > drawing_step)
		{
			drawing_clock.restart();
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			/* Обновляем расположение планет */
			_mutex.lock();
			planets_calculation.GetSnapshot(planets_drawing);
			dt = elapsed_time.getElapsedTime().asSeconds(); //записываем точное прошедшее время в секундах (нужно для вычислений)
			elapsed_time.restart();
			_mutex.unlock();
			
			window.clear();
			window.display();
		}
	}
		
}