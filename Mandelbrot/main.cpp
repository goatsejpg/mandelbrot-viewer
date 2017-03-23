#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "mandelbrot.h"
#include "palette.h"
#include "timestamp.h"
#include "win.h"

void update_win(SDL_Texture* texture, SDL_Renderer* r) {
	SDL_GetRendererOutputSize(r, &Width, &Height);
	SDL_Texture* temp = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
	                                      Width, Height);;
	SDL_SetRenderTarget(r, temp);
	SDL_RenderCopy(r, texture, NULL, NULL);
	SDL_SetRenderTarget(r, NULL);
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
	                            SDL_TEXTUREACCESS_TARGET, Width, Height);

	SDL_SetRenderTarget(r, texture);
	SDL_RenderCopy(r, temp, NULL, NULL);
	SDL_SetRenderTarget(r, NULL);

	win_ratio = Height / static_cast<double>(Width);
	set_min_max();
	SDL_DestroyTexture(temp);
}

int main(int argc, char* argv[]) {
	if (argc > 3) {
		pos = Vec2(std::stod(argv[1], NULL), std::stod(argv[2], NULL));
		zoom = std::stod(argv[3], NULL);
		if (argc > 4) {
			iterations = std::stoi(argv[4], NULL);
			if (iterations < 0) iterations = 0;
		}
	}
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL: Video couldn't be initalized." << std::endl;
		return -1;
	}
	SDL_Window* window = SDL_CreateWindow("Mandelbrot!", SDL_WINDOWPOS_CENTERED,
	                                      SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event evt;

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
	                                         SDL_TEXTUREACCESS_TARGET, Width, Height); 
	SDL_RenderPresent(renderer);
	update_win(texture, renderer);

	bool quit = false;
	bool render_frame = true;
	default_palette();

	bool selecting_zoom = false;
	bool zoom_out = false;
	int mousex_start = 0, mousey_start = 0;
	int mousex = 0, mousey = 0;
	SDL_Rect mouse_rect = {0,0,0,0};

	while (!quit) {
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT || evt.button.button == SDL_BUTTON_RIGHT) {
					SDL_GetMouseState(&mousex_start, &mousey_start);
					mousex = mousex_start; mousey = mousey_start;
					selecting_zoom = true;
					if (evt.button.button == SDL_BUTTON_RIGHT) {
						zoom_out = true;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if ((evt.button.button == SDL_BUTTON_LEFT || evt.button.button == SDL_BUTTON_RIGHT) && selecting_zoom) {
					selecting_zoom = false;
					if (!zoom_out)
						set_min_max_from_rect(mouse_rect);
					else
						zoom = zoom / (mouse_rect.x / static_cast<double>(Width));
					
					render_frame = true;
					zoom_out = false;
				}
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_SPACE:
					// prints complex number right underneath the cursor
					SDL_GetMouseState(&mousex, &mousey);
					std::cout << '(' << minN.x + (mousex *  step.x) << ", " << 
					minN.y + (mousey * step.y) << ')' << std::endl;
					break;
				case SDLK_F5: {
					// screenshots
					SDL_Surface* sshot = SDL_CreateRGBSurface(0, Width, Height, 32, 0x00ff0000, 0x0000ff00,
						0x000000ff, 0xff000000);
					SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
					char file_name[38] = {"screenshots/screenshot-"};
					timestamp(&file_name[23]);
					file_name[33]='.';file_name[34]='b';file_name[35]='m';file_name[36]='p';file_name[37]=0;
					std::cout << file_name << std::endl;
					SDL_SaveBMP(sshot, file_name);
					SDL_FreeSurface(sshot);
					break; }
				case SDLK_F6:
					// resets view
					pos = Vec2(-0.25, 0.0);
					zoom = 2.0;
					render_frame = true;
					break;
				case SDLK_DOWN:
					if (--iterations < 0) iterations = 0;
					std::cout << iterations << " iterations" << std::endl;
					break;
				case SDLK_UP:
					if (++iterations > 0x1000000) iterations = 0x1000000;
					std::cout << iterations << " iterations" << std::endl;
					break;
				case SDLK_F7:
					render_frame = true;
					break;
				}
				break;
			case SDL_WINDOWEVENT:
				if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
					update_win(texture, renderer);
				}
				break;
			}
		}
		
		if (render_frame) {
			update_win(texture, renderer);
			SDL_RenderClear(renderer);
			set_min_max();
			render(renderer, texture);
			render_frame = false;
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		if (selecting_zoom) {
			SDL_GetMouseState(&mousex, &mousey);
			mouse_rect.x = mousex_start; mouse_rect.y = mousey_start;
			mouse_rect.w = mousex - mousex_start; mouse_rect.h = mousey - mousey_start;
			SDL_SetRenderDrawColor(renderer, 255,255,255,255);
			SDL_RenderDrawRect(renderer, &mouse_rect);
			SDL_SetRenderDrawColor(renderer, 0,0,0,255);
		}

		SDL_RenderPresent(renderer);

		SDL_Delay(33);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;	
}
