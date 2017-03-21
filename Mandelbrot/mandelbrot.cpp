#include <SDL2/SDL.h>
#include <complex>
#include <iostream>

#include "mandelbrot.h"
#include "palette.h"

Vec2 pos(-0.25, 0.0);
double zoom = 2;
Vec2 minN, maxN;
Vec2 step;
int iterations = 256;
double iteration_intensity = static_cast<double>(iterations) / 256.0;

void set_min_max(const double& win_ratio, const int& Width, const int& Height) {
	minN.x = pos.x - zoom; maxN.x = pos.x + zoom;
	minN.y = pos.y - (zoom * win_ratio); maxN.y = pos.y + (zoom * win_ratio);
	step.x = (maxN.x - minN.x) / Width;
	step.y = (maxN.y - minN.y) / Height;
}

void set_min_max_from_rect(const SDL_Rect& rect, const double& win_ratio, const int& Width, const int& Height) {
	pos = Vec2(
		(2*minN.x+(rect.x*step.x)+((rect.x+rect.w)*step.x))/2.0,
		(2*minN.y+(rect.y*step.y)+((rect.y+rect.h)*step.y))/2.0
	);
	zoom = abs(rect.w * step.x / 2);
	set_min_max(win_ratio, Width, Height);
}

char func(const int& x, const int& y) {
	std::complex<double> c(minN.x + (step.x * (double)x), minN.y + (step.y * (double)y));
	std::complex<double> z(0.0, 0.0);

	int ct = 0;
	while (ct < iterations) {
		if (abs(z) >= 2.0) break;
		z = z * z + c;
		++ct;
	}

	if (ct == iterations) 
		return 255; // ensures that it returns max even if iterations is a rather incompatible number
	else
		return (char)((double)ct * iteration_intensity);
}

void render(SDL_Renderer* r, SDL_Event* evt, const int& Width, const int& Height, bool& quit, SDL_Texture* texture, const int& pixels) {
	SDL_SetRenderTarget(r, texture);
	int count = 0;
	SDL_Rect rect;
	for (short x = 0; x < Width; ++x) {
		for (short y = 0; y < Height; ++y) {
			char t = func(x,y);
			set_color(r, t);
			SDL_RenderDrawPoint(r, x, y);
		}
		count += Height;
		SDL_RenderPresent(r);
		SDL_SetRenderTarget(r, NULL);
		SDL_RenderCopy(r, texture, NULL, NULL);
		SDL_SetRenderDrawColor(r, 50, 40, 255, 255);
		rect = {0, 0, (int)(Width * (count / (double)pixels)), 5};
		SDL_RenderFillRect(r, &rect);	

		SDL_RenderPresent(r);
		SDL_SetRenderTarget(r, texture);

		if (SDL_PollEvent(evt) && evt->type == SDL_QUIT) {quit = true; goto skip;}
	}
	SDL_RenderPresent(r);
	SDL_SetRenderTarget(r, NULL);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	skip:
		NULL;
}
