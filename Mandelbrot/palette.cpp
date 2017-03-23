#include <SDL2/SDL.h>
#include <cstdint>
#include "palette.h"

#define PSIZE 256

Palette::Palette() {
	for (short i = 0; i < PSIZE; ++i) {
		this->colors[i] = {0,0,0};
	}
}

Palette active_palette;

void default_palette(void) {
	for (short i = 0; i < PSIZE; ++i) {
		active_palette.colors[i].r = 0;
		active_palette.colors[i].g = i >= 256 / 3 ? 255 - abs((i - (2*(256 / 3))) * 3) : 0;
		active_palette.colors[i].b = i < 2 * (256 / 3) ? (255 - abs((i - (256 / 3)) * 3)) / 4: 0;
	}
	active_palette.colors[255] = {0,0,0};
}

void set_color(SDL_Renderer* r, const uint8_t& intensity) {
	SDL_SetRenderDrawColor(r, 
	                       active_palette.colors[255-intensity].r,
	                       active_palette.colors[255-intensity].g,
	                       active_palette.colors[255-intensity].b, 0xff
	);
}
