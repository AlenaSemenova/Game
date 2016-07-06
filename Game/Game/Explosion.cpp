#include "Header.h"

#define end_explosion_step 10
#define image_width 1620
#define image_heigth 130
#define block_width 162
#define block_heigth 130

Explosion::Explosion(int m_step, map<int, Planet>::iterator _first_planet, map<int, Planet>::iterator _second_planet)
{
	step = m_step;
	first_planet = _first_planet;
	second_planet = _second_planet;
}

void Explosion::PrepareDrawing(Texture & expl_texture)
{
	if (step == 0)
	{
		Vector2f scale;
		image.setTexture(expl_texture);	
		image.setOrigin(block_width/ 2, block_heigth / 2);
		scale = Vector2f(2.0f * (first_planet->second.radius + second_planet->second.radius) / block_width,
			2.0f * (first_planet->second.radius + second_planet->second.radius) / block_heigth);
		image.setScale(scale);
	}
	
	image.setPosition((first_planet->second.position + second_planet->second.position ) / 2.0f);
	image.setTextureRect(IntRect(step * block_width, 0, block_width, block_heigth));

	++step;
}
