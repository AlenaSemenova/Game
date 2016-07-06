#pragma once

class Scene: public RenderWindow
{
public:
	Scene(VideoMode mode, const String &title)
		: RenderWindow(mode, title) {}

	map <int, Planet> planets;
	vector <Explosion> explosions;
	View view;
	Sprite background;
	Text time_text;
	Time result_time;
	Texture expl_animation;

	void GetSnapshot(PhysicsEngine & sourse_planets);
	void Initialize(Texture * sky_textures);
	void Draw();
	void DrawGameOver();
	void SetView();
	void ConvertTimeToText(string first_word);
	void InitializeGameOverScene(Text &text);
	void PrepareGameOverTimeText(Text &time_text);
};