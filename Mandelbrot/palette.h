#pragma once

#ifndef PALETTE_H
#define PALETTE_H

#include <SDL2/SDL.h>

struct Palette {
	SDL_Color colors[256];
	Palette();
};

extern Palette active_palette;

void default_palette();

void set_color(SDL_Renderer* r, char intensity);

#endif // PALETTE_H
