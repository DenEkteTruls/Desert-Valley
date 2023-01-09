#pragma once

#include "engine.hpp"



class Menu
{
public:
	Menu(SDL_Renderer *renderer);
	~Menu() { };

	void loadTextures();
	void render();

	bool in_menu = true;
	SDL_Renderer* renderer;

	SDL_Rect banner_dst;
	SDL_Rect continue_dst;
	SDL_Rect new_game_dst;
	SDL_Rect settings_dst;

private:
	SDL_Texture* banner;
	SDL_Texture* continue_button;
	SDL_Texture* new_game_button;
	SDL_Texture* settings_button;
};



Menu::Menu(SDL_Renderer *renderer)
{
	this->renderer = renderer;
	this->loadTextures();
}



void Menu::loadTextures()
{
	this->banner = IMG_LoadTexture(this->renderer, "menu/banner2.png");
	this->continue_button = IMG_LoadTexture(this->renderer, "menu/continue.png");
	this->new_game_button = IMG_LoadTexture(this->renderer, "menu/continue.png");
	this->settings_button = IMG_LoadTexture(this->renderer, "menu/continue.png");

	this->banner_dst.w = 1280; this->banner_dst.h = 768;
	this->continue_dst.x = 90; this->continue_dst.y = 550; this->continue_dst.w = 300; this->continue_dst.h = 70;
	this->new_game_dst.x = 490; this->new_game_dst.y = 550; this->new_game_dst.w = 300; this->new_game_dst.h = 70;
	this->settings_dst.x = 890; this->settings_dst.y = 550; this->settings_dst.w = 300; this->settings_dst.h = 70;
}



void Menu::render()
{
	SDL_RenderCopy(this->renderer, this->banner, NULL, &this->banner_dst);
	SDL_RenderCopy(this->renderer, this->continue_button, NULL, &this->continue_dst);
	SDL_RenderCopy(this->renderer, this->new_game_button, NULL, &this->new_game_dst);
	SDL_RenderCopy(this->renderer, this->settings_button, NULL, &this->settings_dst);
}