#pragma once

#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <SDL2/SDL.h>

struct Vec2 {
	double x,y;
	Vec2() = default;
	using d = const double&;
	Vec2(d _x, d _y): x(_x), y(_y) {}
};

extern Vec2 pos;
extern double zoom;
extern Vec2 minN, maxN;
extern Vec2 step;
extern double iteration_intensity;
extern int iterations;

void set_min_max();
void set_min_max_from_rect(const SDL_Rect& rect);
void mandelbrot(unsigned char** p, int __CORE, int& s, char extra);
void mandelbrot_get_intensity(const int& x, const int& y, unsigned char& pixel);

void render(SDL_Renderer* r, SDL_Texture* texture);

#endif // MANDELBROT_H
