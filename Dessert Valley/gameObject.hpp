#pragma once

#include "engine.hpp"



Uint32 last_time_still; // player standing animation clock (var)
Uint32 last_time_running_right; // player running right animation clock (var)
Uint32 last_time_running_left; // player running left animation clock (var)



class GameObject
{
public:
	GameObject(SDL_Renderer* renderer, const char* imagePath, int w, int h);
	~GameObject() {  }

	void render();
	void loadAnimations();
	void playerEventHandler(int moving_speed, std::vector<GameObject*>* gameObjects);
	GameObject* checkCollision(std::vector<GameObject*>* gameObjects, int x, int y);

	void standingAnimation();
	void runningLeftAnimation();
	void runningRightAnimation();

	int id;
	static int idx;
	int speed = 5;
	const char* imagePath;
	int moved[2] = { 0, 0 };
	bool is_player = false;
	bool is_collidable = false;

	int still_animation_step = 0;
	int running_left_animation_step = 0;
	int running_right_animation_step = 0;

	SDL_Rect dst = { 0, 0, 64, 64 };
	SDL_Texture* texture;
	SDL_Renderer* renderer;

private:
	bool is_animations_loaded = false;

	SDL_Texture* still_anim_1;
	SDL_Texture* still_anim_2;

	std::vector<SDL_Texture*> run_right_anim;
	std::vector<SDL_Texture*> run_left_anim;
};



int GameObject::idx = 0;



GameObject::GameObject(SDL_Renderer* renderer, const char* imagePath, int w, int h)
{
	this->dst.w = w;
	this->dst.h = h;
	this->renderer = renderer;
	this->imagePath = imagePath;
	this->texture = IMG_LoadTexture(this->renderer, imagePath);
	this->id = this->idx++;

	last_time_still = SDL_GetTicks();
}



void GameObject::loadAnimations()
{
	this->still_anim_1 = IMG_LoadTexture(this->renderer, "players/2/front_1.png");
	this->still_anim_2 = IMG_LoadTexture(this->renderer, "players/2/front_2.png");

	for (int i = 1; i < 9; i++)
	{
		std::string filename = "players/2/run_right_" + std::to_string(i) + ".png";
		run_right_anim.push_back(IMG_LoadTexture(this->renderer, filename.c_str()));
	}

	for (int i = 1; i < 9; i++)
	{
		std::string filename = "players/2/run_left_" + std::to_string(i) + ".png";
		run_left_anim.push_back(IMG_LoadTexture(this->renderer, filename.c_str()));
	}
}



GameObject* GameObject::checkCollision(std::vector<GameObject*>* gameObjects, int x, int y)
{
	int px = this->dst.x;
	int py = this->dst.y;

	for (GameObject* gObject : *gameObjects)
	{
		if (gObject->id == this->id || !gObject->is_collidable) continue;
		if (px - x <= gObject->dst.x + gObject->dst.w && px + x >= gObject->dst.x) {
			if (py + y >= gObject->dst.y && py - y <= gObject->dst.y + gObject->dst.h) {
				return gObject;
			}
		}
	}
	
	return nullptr;
}



void GameObject::playerEventHandler(int moving_speed, std::vector<GameObject*>* gameObjects)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	GameObject* colXL = this->checkCollision(gameObjects, this->speed, 0);
	GameObject* colXR = this->checkCollision(gameObjects, -this->speed, 0);
	GameObject* colYB = this->checkCollision(gameObjects, 0, -this->speed);
	GameObject* colYT = this->checkCollision(gameObjects, 0, this->speed);

	std::cout << colXL  << " + " << colXR << " + " << colYB << " + " << colYT << std::endl;


	if (keystate[SDL_SCANCODE_W] && colYT == nullptr) {
		this->moved[1] += this->speed;
	}

	if (keystate[SDL_SCANCODE_D] && colXR == nullptr) {
		this->moved[0] -= this->speed;
	}

	if (keystate[SDL_SCANCODE_S] && colYB == nullptr) {
		this->moved[1] -= this->speed;
	}

	if (keystate[SDL_SCANCODE_A] && colXL == nullptr) {
		this->moved[0] += this->speed;
	}

	if (this->is_player) {
		if (!is_animations_loaded) {
			this->loadAnimations();
			this->is_animations_loaded = true;
		}
		this->standingAnimation();
		this->runningRightAnimation();
		this->runningLeftAnimation();
	}
}



void GameObject::standingAnimation()
{
	if (this->moved[0] == 0 && this->moved[1] == 0) {
		running_left_animation_step = 0;
		running_right_animation_step = 0;
		if (SDL_GetTicks() - last_time_still > 300) {
			if (still_animation_step == 0) {
				texture = still_anim_2;
				still_animation_step = 1;
			}
			else {
				texture = still_anim_1;
				still_animation_step = 0;
			}
			last_time_still = SDL_GetTicks();
		}
	}

}



void GameObject::runningRightAnimation()
{
	if (this->moved[0] < 0) {
		if (SDL_GetTicks() - last_time_running_right > 50) {
			this->texture = run_right_anim.at(running_right_animation_step);
			if (running_right_animation_step < 7) { running_right_animation_step++; }
			else { running_right_animation_step = 0; }
			last_time_running_right = SDL_GetTicks();
		}
	}
}



void GameObject::runningLeftAnimation()
{
	if (this->moved[0] > 0) {
		if (SDL_GetTicks() - last_time_running_left > 50) {
			this->texture = run_left_anim.at(running_left_animation_step);
			if (running_left_animation_step < 7) { running_left_animation_step++; }
			else { running_left_animation_step = 0; }
			last_time_running_left = SDL_GetTicks();
		}
	}
}



void GameObject::render()
{
	SDL_RenderCopy(this->renderer, this->texture, NULL, &dst);
}