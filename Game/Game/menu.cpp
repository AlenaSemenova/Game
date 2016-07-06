#include "Header.h"

#define time_step 20
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 37

inline bool OnButton(Vector2i pos, Sprite & button)
{
	Vector2f left_corner(button.getPosition().x - (BUTTON_WIDTH / 2) * button.getScale().x, button.getPosition().y - (BUTTON_HEIGHT / 2) * button.getScale().y),
		right_corner(button.getPosition().x + (BUTTON_WIDTH / 2) * button.getScale().x, button.getPosition().y + (BUTTON_HEIGHT / 2) * button.getScale().y);
	return (pos.x > left_corner.x && pos.x < right_corner.x && pos.y > left_corner.y &&  pos.y < right_corner.y);
}

void PrepareText(Text &text)
{
	Font *font = new Font;
	font->loadFromFile("Invertor.ttf");
	text.setFont(*font);
	text.setString("Galaxy");
	text.setCharacterSize(300);
	text.setColor(Color::White);
	FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	text.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGTH * 2 / 5));
}

void PrepareStartButton (Sprite &start_button)
{
	start_button.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGTH * 4 / 5));
	start_button.setTextureRect(IntRect(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT));
	start_button.setOrigin(50, 18);
	start_button.setScale(Vector2f(2.0f, 2.0f));
	start_button.setColor(Color(255, 255, 255, 150));
}

void InitializeMenu(Scene & window, Sprite &background, Sprite &start_button, Text &text)
{
	Texture *background_texture = new Texture;
	background_texture->loadFromFile("images/menu.png");
	background.setTexture(*background_texture);

	Texture *button_texture = new Texture;
	button_texture->loadFromFile("images/start.png");
	start_button.setTexture(*button_texture);
	PrepareStartButton(start_button);

	PrepareText(text);
}


void DisplayMenu(Scene & window)
{	
	Sprite background, start_button;
	Text text;
	InitializeMenu(window, background, start_button, text);

	int pressed_rect_top = 2 * (BUTTON_HEIGHT + 20);
	IntRect wait_rect(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT), pressed_rect(0, pressed_rect_top, BUTTON_WIDTH, BUTTON_HEIGHT);
	
	bool is_menu_open = true;
	bool mouse_was_pressed = false;
	Event event;
	Clock elapsed_time, time_pressed;
	Vector2i mouse_position;
	while (is_menu_open && window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
				exit(0);
			}			
		}

		if (Mouse::isButtonPressed(Mouse::Left) && !mouse_was_pressed)
		{
			mouse_position = Mouse::getPosition(window);
			if (OnButton(mouse_position, start_button))
			{
				start_button.setTextureRect(pressed_rect);
				mouse_was_pressed = true;
			}
		}

		else if (mouse_was_pressed)
		{			
			start_button.setTextureRect(wait_rect);
			is_menu_open = false;
		}		

		window.clear();
		window.draw(background);
		window.draw(start_button);
		window.draw(text);
		window.display();

		sleep(milliseconds(time_step) - elapsed_time.getElapsedTime());
		elapsed_time.restart();
	}
}