#include "Header.h"

#define drawing_step 40
#define calculation_step 20

mutex _mutex;
atomic_flag end_game = ATOMIC_FLAG_INIT; 
float dt;

/*  Нить вычислений */
void CalculationThread(PhysicsEngine & planets)
{
	Clock calculation_clock;
	/* Выполняется, пока процесс в main не установил end_game = 1 */
	while (end_game.test_and_set(memory_order_acquire))
	{
		/* Время обновления координат - 20 миллисекунд */
		if (calculation_clock.getElapsedTime().asMilliseconds() > calculation_step)
		{
			_mutex.lock();
			planets.UpdatePosition();
			_mutex.unlock();
		}
		end_game.clear(memory_order_release);
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
	Vector2f mouse_position;
	map<int, Planet>::iterator it_my_planet = planets_drawing.begin(), it_coursor = ++it_my_planet;
	bool mouse_prev_state = 0;
	while (window.isOpen())
	{
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				end_game.test_and_set(memory_order_acquire);
				Thread.join();
				window.close();
			}				
		}

		if (Mouse::isButtonPressed(Mouse::Left) && !mouse_prev_state)
		{
			mouse_position = Vector2f(sf::Mouse::getPosition(window));
			planets_calculation.ChangeCoursorPlanet(mouse_position);
			it_coursor->second.SetСharacteristics(mouse_position, it_my_planet->second.radius,
				it_my_planet->second.mass, Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f));
			mouse_prev_state = 1;
		}

		if (!(Mouse::isButtonPressed(Mouse::Left)) && mouse_prev_state)
			mouse_prev_state = 0;

		if (mouse_prev_state)
			Mouse::setPosition(Vector2i(mouse_position), window);

		/* Шаг отрисовки - 40 миллисекунд */
		if (drawing_clock.getElapsedTime().asMilliseconds() > drawing_step)
		{
			drawing_clock.restart();	

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
		}
	}
		
}