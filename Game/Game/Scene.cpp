#include "Header.h"

#define CHARACTER_TEXT_SIZE 150
#define time_step 20

extern mutex expl_mutex;

/* делает моментальный снимок списка планет для физических вычислений и записывает в copy_planets для рисования  */
void Scene::GetSnapshot(PhysicsEngine &sourse_planets)
{
	map<int, Planet>::iterator i = sourse_planets.planets.begin(), j = planets.begin(), tmp;
	for (; i != sourse_planets.planets.end() && j != planets.end(); ++i)
	{
		while (i != sourse_planets.planets.end() && j != planets.end() && i->first > j->first)
		{
			tmp = j++;
			planets.erase(tmp);
		}
		if (j != planets.end() && i->first == j->first)
		{
			j->second.SetСharacteristics(i->second.getPosition(), i->second.radius, i->second.mass, i->second.velocity, i->second.acceleration);
			++j;
		}
	}
	while (i != sourse_planets.planets.end())
	{
		planets.insert(*i);
		++i;
	}
	while (j != planets.end())
	{
		tmp = j++;
		planets.erase(tmp);
	}														 
}

void Scene::Initialize(Texture * sky_textures)
{
	view = View(Vector2f(float(WINDOW_WIDTH / 2), float(WINDOW_HEIGTH / 2)), Vector2f(float(VIEW_WIDTH), float(VIEW_HEIGTH)));

	background.setTexture(sky_textures[0]);
	FloatRect BGRect = background.getLocalBounds();
	background.setScale(Vector2f(float(WINDOW_WIDTH) / BGRect.width, float(WINDOW_HEIGTH) / BGRect.height));

	expl_animation.loadFromFile("images/ba97fd4ba74d4b4106a0db3484398118.png");

	Font *font = new Font;
	font->loadFromFile("Invertor.ttf");
	time_text.setFont(*font);
	time_text.setCharacterSize(CHARACTER_TEXT_SIZE / 3);
	time_text.setColor(Color::White);
	FloatRect textRect = time_text.getLocalBounds();
	time_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

void Scene::ConvertTimeToText(string first_word)
{
	int int_minutes = int(result_time.asSeconds()) / 60;
	int int_seconds = int(result_time.asSeconds()) % 60;

	char str_minutes[5], str_seconds[5];
	itoa(int_seconds, str_seconds, 10);
	itoa(int_minutes, str_minutes, 10);

	time_text.setString(first_word + " " + string(str_minutes) + "." + string(str_seconds));
}


void Scene::Draw()
{
	draw(background);
	SetView();

	float texture_size = 128.0f;
	for (map<int, Planet>::iterator i = planets.begin(); i != planets.end(); ++i)
	{
		i->second.image->setPosition(i->second.position);
		i->second.image->setScale(2.0f * Vector2f(i->second.radius, i->second.radius) / texture_size);
		i->second.image->setOrigin(Vector2f(texture_size / 2, texture_size / 2));
		draw(*(i->second.image));
	}

	expl_mutex.lock();
	for (int i = 0; i < explosions.size(); ++i)
	{
		explosions[i].PrepareDrawing(expl_animation);
		draw(explosions[i].image);
	}		
	expl_mutex.unlock();

	ConvertTimeToText("Time:");
	time_text.setPosition(view.getCenter() + Vector2f(VIEW_WIDTH / 4.0f, -VIEW_HEIGTH / 2.0f));
	draw(time_text);
}

void PrepareGameOverText(Text &text)
{
	Font *font = new Font;
	font->loadFromFile("Invertor.ttf");
	text.setFont(*font);
	text.setCharacterSize(CHARACTER_TEXT_SIZE);
	text.setColor(Color::White);
	text.setString("Game over");
	text.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGTH * 1 / 10));
	FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

void Scene::PrepareGameOverTimeText(Text &time_text)
{
	FloatRect ttextRect = time_text.getLocalBounds();
	time_text.setOrigin(ttextRect.left + ttextRect.width / 2.0f, ttextRect.top + ttextRect.height / 2.0f);
	time_text.setCharacterSize(CHARACTER_TEXT_SIZE);
	time_text.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGTH * 1 / 5));

	ConvertTimeToText("Your result:");
}

void Scene::InitializeGameOverScene(Text &text)
{
	Texture *background_texture = new Texture;
	background_texture->loadFromFile("images/hq-wallpapers_ru_space_4102_1920x1200.png");
	background.setTexture(*background_texture);
	background.scale(2.0f, 2.0f);

	PrepareGameOverText(text);
	PrepareGameOverTimeText(time_text);
}

void Scene::DrawGameOver()
{
	Text text;
	InitializeGameOverScene(text);
	setView(getDefaultView());

	Event event;
	Clock elapsed_time;
	while (isOpen())
	{
		while (pollEvent(event))
		{
			if (event.type == Event::Closed)
				close();
		}

		clear();
		draw(background);
		draw(time_text);
		draw(text);
		display();
		
		sleep(milliseconds(time_step) - elapsed_time.getElapsedTime());
		elapsed_time.restart();
	}
}

void Scene::SetView()
{
	Vector2f new_view_pos;
	if (planets[0].getPosition().y < VIEW_HEIGTH / 2)
		new_view_pos.y = VIEW_HEIGTH / 2;
	else if (planets[0].getPosition().y > WINDOW_HEIGTH - (VIEW_HEIGTH / 2))
		new_view_pos.y = WINDOW_HEIGTH - (VIEW_HEIGTH / 2);
	else
		new_view_pos.y = planets[0].getPosition().y;
	
	if (planets[0].getPosition().x < VIEW_WIDTH / 2)
		new_view_pos.x = VIEW_WIDTH / 2;
	else if (planets[0].getPosition().x > WINDOW_WIDTH - (VIEW_WIDTH / 2))
		new_view_pos.x = WINDOW_WIDTH - (VIEW_WIDTH / 2);
	else
		new_view_pos.x = planets[0].getPosition().x;

	view.setCenter(new_view_pos);
}

