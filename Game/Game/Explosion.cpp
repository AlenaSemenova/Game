#include "Header.h"

#define end_explosion_step 10
#define image_width 1620
#define image_heigth 130
#define block_width 162
#define block_heigth 130

Explosion::Explosion(int m_step, map<int, Planet>::iterator m_i, map<int, Planet>::iterator m_j)
{
	step = m_step;
	i = m_i;
	j = m_j;
}

void Explosion::PrepareDrawing(Texture & expl_texture)
{
	if (step == 0)
	{
		Vector2f scale;
		image.setTexture(expl_texture);	
		image.setOrigin(block_width/ 2, block_heigth / 2);
		scale = Vector2f(2.0f * (i->second.radius + j->second.radius) / block_width, 2.0f * (i->second.radius + j->second.radius) / block_heigth);
		image.setScale(scale);
	}
	
	image.setPosition((i->second.position + j->second.position ) / 2.0f);
	image.setTextureRect(IntRect(step * block_width, 0, block_width, block_heigth));

	++step;
}
