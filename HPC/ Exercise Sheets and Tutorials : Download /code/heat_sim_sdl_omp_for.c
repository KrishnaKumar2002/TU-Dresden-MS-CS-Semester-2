#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>
#include <SDL2/SDL.h>
#include <signal.h>

// Simulation parameters
// =========================
// Grid size
#define N 1002
// Alpha (thermal diffusivity)
#define ALPHA 0.8
// Time step
#define DT 0.3
// Space step
#define DX 1.0
#define HEAT_SOURCE_RADIUS 20
#define HEAT_SOURCE_TEMP 100.0

#ifndef DELAY
#define DELAY 0
#pragma message("Using no DELAY. to set custom delay on top left tile, compile with -DDELAY=<value>")
#else
#pragma message("Using user-defined DELAY on top left tile")
#endif
static long long int delay=DELAY;

#ifdef SHOW_ITERATION
static int show_delay=2;
static int show_thread=0;
#define DESCRIPTION "(Blue=Lower Iteration, Red=Higher Iteratio)"
#pragma message("SDL will show the current iteration for each tile, ignoring -DSHOW_THREAD")
#else
static int show_delay=0;
#ifdef SHOW_THREADS
static int show_thread=1;
#pragma message("SDL will show the current thread for each tile")
#define DESCRIPTION "(Blue=Lower Thread Number, Red=Higher Thread Number)"
#else
static int show_thread=0;
#pragma message("SDL will show the computed values of u_new. These might be out of sync, due to task parallelism. Compile with -DSHOW_ITERATION to show iteration count instead. Use -DSHOW_THREADS to show thread IDs.")
#define DESCRIPTION "(Blue=Cold, Red=Hot)"
#endif
#endif


#define stringify_helper(x) #x
#define STRINGIFY(x) stringify_helper(x)


double u[N][N] = {0.0};
double u_new[N][N] = {0.0};
double u_display[N][N] = {0.0};

volatile int render_step = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;


void compute_heat(double u[N][N] , double u_new[N][N], double u_display[N][N]) {
	#pragma omp parallel for schedule(runtime)
	for (int i = 1; i < N-1; i++) {
		for (int j = 1; j< N-1; j++) {

			u_new[i][j] = u[i][j] + ALPHA * DT / (DX*DX) *
				(u[i+1][j] + u[i-1][j] + u[i][j+1] + u[i][j-1] - 4*u[i][j]);
			if (show_delay) {
				u_display[i][j]+=show_delay;
				if (u_display[i][j]>=100)
					u_display[i][j]=u_display[i][j]-100;
			}
			else if (show_thread) {
				u_display[i][j]=100.0*omp_get_thread_num()/omp_get_num_threads();
			}
			else {
				u_display[i][j]=u_new[i][j];
			}
								
			if ((i == 1) && (j == 1)) // delay everything with bogus work to simulate load
				for (volatile long long int k=0; k<delay; k++);
		}
	}
}

void add_heat_source(int col, int row, double temp, int radius) {	
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			int py = row + i;
			int px = col + j;
			
			// Check bounds
			if (py >= 0 && py < N && px >= 0 && px < N) {
				// Gaussian-like falloff from center
				double dist_sq = (double)(i*i + j*j);
				double radius_sq = (double)(radius * radius);
				double falloff = exp(-2.0 * dist_sq / radius_sq);
				
				u[py][px] += temp * falloff;
			}
		}
	}
	
}

static unsigned char pixels[N * N * 3];
void display_frame(int step) {
	
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			double normalized = u_display[i][j] / 100.0;
			if (normalized < 0.0) normalized = 0.0;
			if (normalized > 1.0) normalized = 1.0;
			
			unsigned char r = (unsigned char)(255.0 * normalized);
			unsigned char g = 0;
			unsigned char b = (unsigned char)(255.0 * (1.0 - normalized));
			
			int idx = (i * N + j) * 3;
			pixels[idx + 0] = r;
			pixels[idx + 1] = g;
			pixels[idx + 2] = b;
		}
	}
		
	SDL_UpdateTexture(texture, NULL, pixels, N * 3);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/* mouse drag support: track left-button state and last added grid cell to avoid flooding */
static int mouse_down = 0;
static int last_mouse_col = -1;
static int last_mouse_row = -1;
void handle_events(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			exit(0);
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				mouse_down = 1;
				/* add at initial press */
				int render_w, render_h;
				SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
				int grid_col = (int)((double)event.button.x / render_w * N);
				int grid_row = (int)((double)event.button.y / render_h * N);
				if (grid_col < 0) grid_col = 0;
				if (grid_col >= N) grid_col = N - 1;
				if (grid_row < 0) grid_row = 0;
				if (grid_row >= N) grid_row = N - 1;
				last_mouse_col = grid_col;
				last_mouse_row = grid_row;
				add_heat_source(grid_col, grid_row, HEAT_SOURCE_TEMP, HEAT_SOURCE_RADIUS);
			}
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				mouse_down = 0;
				last_mouse_col = last_mouse_row = -1;
			}
		} else if (event.type == SDL_MOUSEMOTION) {
			/* while left button is held, add sources along the drag path.
			   avoid adding multiple times to same grid cell by tracking last cell. */
			if (mouse_down) {
				int render_w, render_h;
				SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
				int grid_col = (int)((double)event.motion.x / render_w * N);
				int grid_row = (int)((double)event.motion.y / render_h * N);
				if (grid_col < 0) grid_col = 0;
				if (grid_col >= N) grid_col = N - 1;
				if (grid_row < 0) grid_row = 0;
				if (grid_row >= N) grid_row = N - 1;
				if (grid_col != last_mouse_col || grid_row != last_mouse_row) {
					last_mouse_col = grid_col;
					last_mouse_row = grid_row;
					add_heat_source(grid_col, grid_row, HEAT_SOURCE_TEMP, HEAT_SOURCE_RADIUS);
				}
			}
         }
     }
}

int init_sdl(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return -1;
	}
	
	window = SDL_CreateWindow(
		"Heat Dissipation "DESCRIPTION" | Left-Click to Add Heat",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		N, N,
		SDL_WINDOW_SHOWN
	);
	if (!window) {
		printf("Window creation failed: %s\n", SDL_GetError());
		return -1;
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		printf("Renderer creation failed: %s\n", SDL_GetError());
		return -1;
	}
	
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, N, N);
	if (!texture) {
		printf("Texture creation failed: %s\n", SDL_GetError());
		return -1;
	}
	
	printf("SDL initialized successfully\n");
	return 0;
}

void cleanup_sdl(void) {
	if (texture) SDL_DestroyTexture(texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
}

// Simulation thread function
void* simulation_thread(void* arg) {
            while (1) {
				/* from u to u_new */
				compute_heat(u, u_new,  u_display);
				/* from u_new to u */
				compute_heat(u_new, u,  u_display);
            } /* while (1) */
    return NULL;
}

static void handle_signal(int sig) {
    exit(0);
}

int main(void) {
    /* install signal handlers to gracefully stop on Ctrl-C / terminate */
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* numeric stability check for explicit scheme (2D): DT <= DX*DX / (4*ALPHA) */
	double dt_max = (DX * DX) / (4.0 * ALPHA);
	if (DT > dt_max) {
		fprintf(stderr, "ERROR: DT=%.6f exceeds stability limit DT_max=%.6f (DX=%.3f, ALPHA=%.3f).\n",
				(double)DT, dt_max, (double)DX, (double)ALPHA);
		fprintf(stderr, "Reduce DT or use an implicit solver to avoid numerical blow-up.\n");
		return 1;
	}

	printf("=== HEAT DISSIPATION WITH OMP PARALLEL FOR (THREADED) ===\n");
    printf("Compiled with N=" STRINGIFY(N) ", ALPHA=" STRINGIFY(ALPHA) ", DT=" STRINGIFY(DT) "\n");
    printf("Ensure that DT <= DX*DX / (4*ALPHA) for numerical stability.\n");
    printf("Left-click in the window to add heat sources\n\n");
	printf("Please note that adding heat sources and drawing screen content is **NOT** synchronized with the simulation steps!\n");
    printf("You can use the environment variable OMP_SCHEDULE to control scheduling.\n");
	
	if (init_sdl() < 0) {
		return 1;
	}
	
	// Initialize with central hot spot
	int hot_size = 5;
	for(int i = -hot_size; i < hot_size; i++) {
		for(int j = -hot_size; j < hot_size; j++) {
			u[(N/2)+i][(N/2)+j] = 100.0;
		}
	}
	
	display_frame(0);
	SDL_Delay(500);
	
	// Start simulation thread
	pthread_t sim_tid;
	pthread_create(&sim_tid, NULL, simulation_thread, NULL);
	
	// RENDERING LOOP - main thread
	int frame = 0;
	while (1) {
		display_frame(render_step);
		handle_events();
		SDL_Delay(10); 
	}
	return 0;
}
