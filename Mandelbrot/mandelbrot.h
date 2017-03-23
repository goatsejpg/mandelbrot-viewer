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

void set_min_max(const double& win_ratio, const int& Width, const int& Height);
void set_min_max_from_rect(const SDL_Rect& rect, const double& win_ratio, const int& Width, const int& Height);
void mandelbrot(unsigned char** p, int __CORE, const int& Height, int& step, char extra);
void func(int x, int y, unsigned char& pixel);

void render(SDL_Renderer* r, SDL_Event* evt, const int& Width, const int& Height, bool& play, SDL_Texture* texture, const int& pixels);

#endif // MANDELBROT_H
