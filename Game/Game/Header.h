#pragma once
#include <iostream>
#include <map>
#include <SFML\Graphics.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <cstdlib>
#include <cmath>

using namespace sf;
using namespace std;

#include "Planets.h"
#include "Explosion.h"
#include "PhysicsEngine.h"
#include "Scene.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGTH 700
#define VIEW_WIDTH 500
#define VIEW_HEIGTH 350
#define NUM_PLANETS_TEXTURES 9
#define NUM_SKY_TEXTURES 5



