/*
* Started: 26.02.2022
* Author: Casper Nag
*/


#include "engine.hpp"

Engine engine("Dessert Valley", 1280, 768);
GameObject adam(engine.renderer, "players/2/front_1.png", 64, 73);
GameObject white(engine.renderer, "buildings/houses/a.png", 64, 64);
Menu menu(engine.renderer);

int main(int argc, char** argv)
{
	adam.dst.x = 64 * 10;
	adam.dst.y = 64 * 6;
	adam.is_player = true;
	
	engine.loadTilemap("levels/1.txt", "levels/1_objects.txt");
	engine.addGameObjects({ &white, &adam });
	engine.menu = &menu;

	SDL_Event event;
	while (engine.running)
	{
		engine.eventHandler(&event);
		
		if (!engine.menu->in_menu) {
			white.dst = engine.getCurrentTile(&adam)->dst;
			adam.playerEventHandler(engine.fpsSpeed);
		}

		engine.render();
		engine.scale_tilemap(&adam);
		engine.fps_locker(60);
	}

	engine.close();
	return 0;
}