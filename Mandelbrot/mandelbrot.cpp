#include <SDL2/SDL.h>
#include <complex>
#include <iostream>
#include <thread>

const int __CORES = std::thread::hardware_concurrency();

#include "mandelbrot.h"
#include "palette.h"
#include "win.h"

Vec2 pos(-0.25, 0.0);
double zoom = 2.0;
Vec2 minN, maxN;
Vec2 step;
int iterations = 256;
double iteration_intensity = static_cast<double>(iterations) / 256.0;

void set_min_max() {
	minN.x = pos.x - zoom; maxN.x = pos.x + zoom;
	minN.y = pos.y - (zoom * win_ratio); maxN.y = pos.y + (zoom * win_ratio);
	step.x = (maxN.x - minN.x) / static_cast<double>(Width);
	step.y = (maxN.y - minN.y) / static_cast<double>(Height);
}

void set_min_max_from_rect(const SDL_Rect& rect) {
	pos = Vec2(
		(2.0*minN.x+(rect.x*step.x)+((rect.x+rect.w)*step.x))/2.0,
		(2.0*minN.y+(rect.y*step.y)+((rect.y+rect.h)*step.y))/2.0
	);
	zoom = (rect.w * step.x / 2.0); if (zoom < 0.0) zoom *= -1.0;
	set_min_max();
}

void mandelbrot_get_intensity(const int& x, const int& y, unsigned char& pixel) {
	std::complex<double> c(minN.x + (step.x * static_cast<double>(x)), minN.y + (step.y * static_cast<double>(y)));
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
		pixel = static_cast<unsigned char>(ct * iteration_intensity);
}

void mandelbrot(unsigned char** p, int __CORE, int& s, char extra) {
	for (int x = s * __CORE; x < (s * __CORE) + s + extra; ++x) {
		for (int y = 0; y < Height; ++y) {
			mandelbrot_get_intensity(x, y, p[x][y]);
		}
	}
}

void render(SDL_Renderer* r, SDL_Texture* texture) {
	int start = SDL_GetTicks(), end;
	SDL_SetRenderTarget(r, texture);
	unsigned short temp = 0;		
	
	unsigned char** p = new unsigned char*[Width];
	for (std::size_t i = 0; i < Width; ++i)
		p[i] = new unsigned char[Height] {0};
	std::thread* threads[__CORES];

	int s = Width / __CORES;
	for (int t = 0; t < __CORES; ++t) {
		char extra = 0;
		if (Width % s && __CORES % 2) extra = 1;
		threads[t] = new std::thread(mandelbrot, p, t, std::ref(s), extra);
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

	for (std::size_t t = 0; t < __CORES; ++t)
		delete threads[t];
	for (std::size_t i = 0; i < Width; ++i)
		delete[] p[i];
	end = SDL_GetTicks();
	std::cout << end - start << " ms" << std::endl;
}
