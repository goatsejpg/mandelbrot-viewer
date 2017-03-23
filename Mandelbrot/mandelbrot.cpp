#include <SDL2/SDL.h>
#include <complex>
#include <iostream>
#include <thread>

const int __CORES = std::thread::hardware_concurrency();

#include "mandelbrot.h"
#include "palette.h"

Vec2 pos(-0.25, 0.0);
double zoom = 2.0;
Vec2 minN, maxN;
Vec2 step;
int iterations = 256;
double iteration_intensity = static_cast<double>(iterations) / 256.0;

void set_min_max(const double& win_ratio, const int& Width, const int& Height) {
	minN.x = pos.x - zoom; maxN.x = pos.x + zoom;
	minN.y = pos.y - (zoom * win_ratio); maxN.y = pos.y + (zoom * win_ratio);
	step.x = (maxN.x - minN.x) / (double)Width;
	step.y = (maxN.y - minN.y) / (double)Height;
}

void set_min_max_from_rect(const SDL_Rect& rect, const double& win_ratio, const int& Width, const int& Height) {
	pos = Vec2(
		(2.0*minN.x+(rect.x*step.x)+((rect.x+rect.w)*step.x))/2.0,
		(2.0*minN.y+(rect.y*step.y)+((rect.y+rect.h)*step.y))/2.0
	);
	zoom = (rect.w * step.x / 2.0); if (zoom < 0.0) zoom *= -1.0;
	set_min_max(win_ratio, Width, Height);
}

void func(int x, int y, unsigned char& pixel) {
	std::complex<double> c(minN.x + (step.x * (double)x), minN.y + (step.y * (double)y));
	std::complex<double> z(0.0, 0.0);

	int ct = 0;
	while (ct < iterations) {
		if (abs(z) >= 2.0) break;
		z = z * z + c;
		++ct;
	}

	if (ct == iterations) 
		pixel =  255;
		// ensures that it returns max even if iterations is a rather incompatible number
	else 
		pixel = (unsigned char)((double)ct * iteration_intensity);
}

void mandelbrot(unsigned char** p, int __CORE, const int& Height, int& step, char extra) {
	for (int x = step * __CORE; x < (step * __CORE) + step + extra; ++x) {
		for (int y = 0; y < Height; ++y) {
			func(x, y, p[x][y]);
		}
	}
}

void render(SDL_Renderer* r, SDL_Event* evt, const int& Width, const int& Height, bool& quit, SDL_Texture* texture, const int& pixels) {
	int start = SDL_GetTicks(), end;
	SDL_SetRenderTarget(r, texture);
	unsigned short temp = 0;		

	//unsigned char p[Width][Height];
	unsigned char** p = new unsigned char*[Width];
	for (std::size_t i = 0; i < Width; ++i)
		p[i] = new unsigned char[Height]; 
	std::thread* threads[__CORES];

	int step = Width / __CORES;
	for (int t = 0; t < __CORES; ++t) {
		char extra = 0;
		if (Width % step && __CORES % 2) extra = 1;
		threads[t] = new std::thread(mandelbrot, p, t, std::ref(Height), std::ref(step), extra);
	}

	for (std::size_t t = 0; t < __CORES; ++t) {
		threads[t]->join();
	}

	for (std::size_t x = 0; x < Width; ++x) {
		for (std::size_t y = 0; y < Height; ++y) {
			set_color(r, p[x][y]);
			SDL_RenderDrawPoint(r, x, y);
		}
	}

	SDL_RenderPresent(r);

	SDL_RenderPresent(r);
	SDL_SetRenderTarget(r, NULL);
	SDL_RenderCopy(r, texture, NULL, NULL);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);

	for (std::size_t t = 0; t < __CORES; ++t)
		delete threads[t];
	for (std::size_t i = 0; i < Width; ++i)
		delete[] p[i];
	end = SDL_GetTicks();
	std::cout << end - start << " ms" << std::endl;
}
