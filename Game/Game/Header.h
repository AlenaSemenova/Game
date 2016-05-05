#pragma once
#include <iostream>
#include <map>
#include <SFML\Graphics.hpp>
#include <thread>
#include <atomic>
#include <mutex>

using namespace sf;
using namespace std;

#include "Planets.h"
#include "PhysicsEngine.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGTH 600
#define VIEW_WIDTH 400
#define VIEW_HEIGTH 300
#define NUM_PLANETS_TEXTURES 2
