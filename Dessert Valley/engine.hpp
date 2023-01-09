#pragma once

#include "engine.hpp"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "gameObject.hpp"
#include "menu.hpp"



static Uint32 next_time; // fps_locker (var)
std::vector<std::string> split(std::string s, char delim); // split (method decl)


class Engine
{
public:
	Engine(const char* title, int w, int h);

	int w, h;
	bool running = false;
	int fpsSpeed = 5;
	int framerate;
	std::ifstream file;
	int map_width;
	Menu *menu;

	void eventHandler(SDL_Event *event);
	void addGameObject(GameObject* gObject);
	void addGameObject(std::initializer_list<GameObject*> gObjects);
	void addGameObjects(std::initializer_list<GameObject*> gObjects);
	void fps_locker(int framerate);

	void loadTilemap(const char* tilemapFile, const char *objectsFile);
	void addTile(const char* tilePath, int x, int y);
	GameObject *getCurrentTile(GameObject* gameObject);
	void scale_tilemap(GameObject* player);
	std::string getCollision(GameObject* player);
	
	void renderTilemap();
	void renderGameObjects();
	void render();

	void close();

	SDL_Window* window;
	SDL_Renderer* renderer;

	std::vector<GameObject> tiles;
	std::vector<GameObject*> gObjects;
};



Engine::Engine(const char* title, int w, int h)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG || IMG_INIT_PNG);
	this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE);
	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	this->running = true;
	this->w = w;
	this->h = h;
}



void Engine::eventHandler(SDL_Event *event)
{
	while (SDL_PollEvent(event))
	{
		switch (event->type)
		{
		case SDL_QUIT:
			this->running = false;
			break;

		case SDL_KEYDOWN:
			switch (event->key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				this->menu->in_menu = !this->menu->in_menu;
				break;

			default:
				break;
			}
		default:
			break;
		}
	}
}



void Engine::fps_locker(int framerate)
{
	this->framerate = framerate;
	this->fpsSpeed = this->framerate / 12;
	this->w = SDL_GetWindowSurface(window)->w;
	this->h = SDL_GetWindowSurface(window)->h;

	Uint32 now = SDL_GetTicks();

	if (next_time > now) {
		SDL_Delay(next_time - now);
	}

	next_time += 1000 / framerate;
}



/* ---------- TILEMAP ----------- */



void Engine::addTile(const char* tilePath, int x, int y)
{
	GameObject tile(this->renderer, tilePath, 64, 64);
	tile.dst.x = 64 * x;
	tile.dst.y = 64 * y;
	this->tiles.push_back(tile);
}



void Engine::loadTilemap(const char* tilemapFile, const char *objectsFile)
{
	int x = 0;
	int y = 0;
	std::string line;
	file.open(tilemapFile);
	int i = 0;

	while (std::getline(this->file, line))
	{
		std::vector<std::string> v = split(line, ',');
		for (std::string a : v)
		{
			if (a == "s") { this->addTile("tiles/sand.png", x, y); }
			else if (a == "g") { this->addTile("tiles/grass.png", x, y); }
			else if (a == "w") { this->addTile("tiles/water.png", x, y); }
			else if (a == "r") { this->addTile("tiles/rock.png", x, y); }
			else if (a == "b_g") { this->addTile("tiles/b_grass.png", x, y); }
			else if (a == "bl_g") { this->addTile("tiles/bl_grass.png", x, y); }
			else if (a == "br_g") { this->addTile("tiles/br_grass.png", x, y); }
			else if (a == "l_g") { this->addTile("tiles/l_grass.png", x, y); }
			else if (a == "r_g") { this->addTile("tiles/r_grass.png", x, y); }
			else if (a == "t_g") { this->addTile("tiles/t_grass.png", x, y); }
			else if (a == "tr_g") { this->addTile("tiles/tr_grass.png", x, y); }
			else if (a == "tl_g") { this->addTile("tiles/tl_grass.png", x, y); }
			x++;
		}
		if (i == 0) { map_width = x + 1; }
		y++;
		x = 0;
		i++;
	}
	this->file.close();

	line = "";
	file.open(objectsFile);

	while (std::getline(this->file, line))
	{
		std::vector<std::string> v = split(line, ',');
		std::vector<std::string> h = split(v.at(1), 'x');
		std::vector<std::string> k = split(v.at(2), 'x');
		std::string l = v.at(3);

		GameObject *tmp = new GameObject(this->renderer, v.at(0).c_str(), std::stoi(k.at(0)), std::stoi(k.at(1)));
		tmp->dst.x = std::stoi(h.at(0));
		tmp->dst.y = std::stoi(h.at(1));
		if (l == "true" || l == " true") tmp->is_collidable = true;
		this->addGameObject(tmp);
	}
	this->file.close();
}



GameObject* Engine::getCurrentTile(GameObject* gObject)
{
	GameObject* tmp_ret = nullptr;

	int abs_x = INT_MAX;
	int abs_y = INT_MAX;
	int closest_x = INT_MAX;
	int closest_y = INT_MAX;

	for (int i = 0; i < this->map_width; i++)
	{
		int abs = std::abs(gObject->dst.x - tiles.at(i).dst.x);
		if (abs < abs_x) {
			closest_x = tiles.at(i).dst.x;
			abs_x = abs;
		}
	}

	for (int i = 0; i < tiles.size(); i += this->map_width)
	{
		int abs = std::abs(gObject->dst.y - tiles.at(i).dst.y);
		if (abs < abs_y) {
			closest_y = tiles.at(i).dst.y;
			abs_y = abs;
		}
	}

	for (GameObject tile : this->tiles)
	{
		if (tile.dst.x == closest_x && tile.dst.y == closest_y) {
			tmp_ret = &tile;
			break;
		}
	}

	return tmp_ret;
}


void Engine::scale_tilemap(GameObject* player)
{
	for (int i = 0; i < this->tiles.size(); i++)
	{
		this->tiles.at(i).dst.x += player->moved[0];
		this->tiles.at(i).dst.y += player->moved[1];
	}

	for (int i = 0; i < this->gObjects.size(); i++)
	{
		if (this->gObjects.at(i) != player) {
			this->gObjects.at(i)->dst.x += player->moved[0];
			this->gObjects.at(i)->dst.y += player->moved[1];
		}
	}

	player->moved[0] = 0;
	player->moved[1] = 0;
}




/* --------------- GameObjects -------------- */



void Engine::addGameObject(GameObject* gObject)
{
	this->gObjects.push_back(gObject);
}



void Engine::addGameObject(std::initializer_list<GameObject*> gObjects)
{
	for (int i = 0; i < gObjects.size(); i++)
	{
		this->gObjects.push_back(*(gObjects.begin() + i));
	}
}



void Engine::addGameObjects(std::initializer_list<GameObject*> gObjects)
{
	for (int i = 0; i < gObjects.size(); i++)
	{
		this->gObjects.push_back(*(gObjects.begin() + i));
	}
}



/* ------------- RENDERING ------------ */



void Engine::renderGameObjects()
{
	for (GameObject* gObject : gObjects)
	{
		gObject->render();
	}
}



void Engine::renderTilemap()
{
	for (GameObject tile : this->tiles)
	{
		tile.render();
	}
}



void Engine::render()
{
	SDL_SetRenderDrawColor(this->renderer, 200, 226, 238, 255);
	SDL_RenderClear(this->renderer);

	if (this->menu->in_menu) {
		this->menu->render();
	}
	else {
		this->renderTilemap();
		this->renderGameObjects();
	}

	SDL_RenderPresent(this->renderer);
}



void Engine::close()
{
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);

	SDL_Quit();
}






/* ---------------- OTHER FUNCTIONS ----------------- */


std::vector<std::string> split(std::string s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}
	return result;
}