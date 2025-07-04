#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "utils.h"
#include "render.h"
#include "texture.h"
#include "entities.h"

struct state_s state;


u8 MAP[MAP_SIZE*MAP_SIZE] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,4,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

clock_t clock1;
clock_t clock2;
bool w = false;
bool s = false;
bool a = false;
bool d = false;

void fixedUpdate() {
	state.fixed_delta_time = difftime(clock(),clock1)/CLOCKS_PER_SEC;
	clock1 = clock();


	#ifdef __linux
	system("clear"); 
	#else
	system("cls");
	#endif

	printf("%f fps\n",1/state.delta_time);
	printf("%f fixed fps\n",1/state.fixed_delta_time);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			state.quit = true;	
		}
		if (event.key.keysym.scancode == SDL_SCANCODE_W&&event.key.state==SDL_PRESSED) {
			w = true;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_W&&event.key.state==SDL_RELEASED) {
			w = false;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_S&&event.key.state==SDL_PRESSED) {
			s = true;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_S&&event.key.state==SDL_RELEASED) {
			s = false;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_A&&event.key.state==SDL_PRESSED) {
			a = true;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_A&&event.key.state==SDL_RELEASED) {
			a = false;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_D&&event.key.state==SDL_PRESSED) {
			d = true;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_D&&event.key.state==SDL_RELEASED) {
			d = false;
		}

	}

		
	
		const u8 *key = SDL_GetKeyboardState(NULL);
		
		SDL_GetMouseState(&state.mousex,&state.mousey);
		SDL_WarpMouseInWindow(state.window,WINDOW_WIDTH/2,WINDOW_HEIGHT/2);

		state.dirangle -= (state.mousex-(WINDOW_WIDTH/2))*ROT_SPEED;	
		state.pan_y += (state.mousey-(WINDOW_HEIGHT/2))*PAN_SPEED*Y_RES;
		if (state.pan_y>Y_RES/2) state.pan_y=Y_RES/2;
		if (state.pan_y<-(Y_RES/2)) state.pan_y=-(Y_RES/2);
		
	

		state.dir.x = cos(state.dirangle);
		state.dir.y = sin(state.dirangle);
	    state.plane = (vec2f) {state.dir.y*FOV_FACTORX,-state.dir.x*FOV_FACTORX};
		
		if (w) {
			state.pos.x += state.dir.x*MOVE_SPEED*state.fixed_delta_time;
			state.pos.y += state.dir.y*MOVE_SPEED*state.fixed_delta_time;
		}

		if (s) {
			state.pos.x -= state.dir.x*MOVE_SPEED*state.fixed_delta_time;
			state.pos.y -= state.dir.y*MOVE_SPEED*state.fixed_delta_time;
		}

		if (a) {
			state.pos.x -= state.dir.y*MOVE_SPEED*state.fixed_delta_time;
			state.pos.y += state.dir.x*MOVE_SPEED*state.fixed_delta_time;
		}

		if (d) {
			state.pos.x += state.dir.y*MOVE_SPEED*state.fixed_delta_time;
			state.pos.y -= state.dir.x*MOVE_SPEED*state.fixed_delta_time;
		}


	updateEntities();
	

}



int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL init error");
		return 1;
	}

	for (int x = 0; x < X_RES*Y_RES; x++) {
		state.pixels[x] = 0x00000000;
	}
	
	loadTextures();
	setupEntities();

	state.window =
		SDL_CreateWindow(
			":3",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_MOUSE_FOCUS
		);
	state.renderer =
		SDL_CreateRenderer(
			state.window,
			-1,
			SDL_RENDERER_ACCELERATED
		);

	state.texture =
		SDL_CreateTexture(
			state.renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			X_RES,
			Y_RES
		);

	SDL_ShowCursor(SDL_DISABLE);

	state.pos.x = 4.0f;
	state.pos.y = 5.0f;
	state.dir = normalize(((vec2f) {1.0f,-1.0f}));
	state.dirangle = 1;
	state.plane = (vec2f) {state.dir.y*FOV_FACTORX,-state.dir.x*FOV_FACTORX};
	state.mousex = 0;
	state.mousey = 0;

	f32 fixed_timec;

	vec2f move;

	clock2 = clock();
	while (!state.quit) {



		state.delta_time = difftime(clock(),clock2)/CLOCKS_PER_SEC;
		clock2 = clock();
	
		fixed_timec += state.delta_time;
		if (fixed_timec>=1.0f/60.0f) {
			fixed_timec=0;
			fixedUpdate();
		}

		SDL_RenderClear(state.renderer);
		
		memset(state.pixels,0,sizeof(state.pixels));
		render();

		SDL_UpdateTexture(state.texture,
			NULL,
			state.pixels,
			X_RES * 4
		);

		SDL_SetRenderTarget(state.renderer, NULL);
    	SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 0xFF);
    	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);
		
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
 
		SDL_RenderPresent(state.renderer);

		//state.quit = true;

	}
	
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
	return 0;
}



