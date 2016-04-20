#include "Header.h"

int main()
{
	vector<Planet> planets_drawing;
	PhysicsEngine planets_calculation;
	planets_calculation.Initialize();

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Game");
	Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (clock.getElapsedTime().asMilliseconds() > time_step)
		{
			//UpdatePosition(planets);
			clock.restart();
		}

		window.clear();
		window.display();
	}
}