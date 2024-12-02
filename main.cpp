#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define SCREEN_WIDTH 41
#define SCREEN_HEIGHT 15
#define FROG_HEIGHT 1
#define FROG_WIDTH 1
#define CAR_SPEED_VAR 2

struct GameState {
	int quit;
	int frog_y = SCREEN_HEIGHT - 2;
	int frog_x = SCREEN_WIDTH/2;
	int frog_color;
	int move;
	char map[SCREEN_HEIGHT][SCREEN_WIDTH] = {' '};
	int timer = 0;
	int points = 0;
	int radius = CAR_SPEED_VAR + 1;
	int collisionDetected = 0; 
	char frog_symbol;
	char car_symbol;
	int car_speed_variable;
	int bouncing_car_color;
	int wrapping_car_color;
	int max_cars;
	int border_symbol;
	int lane_separator;
	int max_time;
};

struct Car {
	int car_x;
	int car_y;
	char direction;
	int speed; 
	char type; // 'w' - wrapping, 'b' - bouncing
	char symbol;
	int color;
	int initialized = 0;// 1 - initialized, 0 - not initialized
};

//FUNCTION TOO LONG
void drawMap(struct GameState* gameState, struct Car* cars) {
	attron(COLOR_PAIR(1));
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 2; j < SCREEN_HEIGHT; j++) {
			if (j % 2 == 0) {
				gameState->map[j][i] = gameState->lane_separator;
			}
		}
		gameState->map[0][i] = gameState->border_symbol;
		gameState->map[SCREEN_HEIGHT-1][i] = gameState->border_symbol;
	}
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		gameState->map[i][0] = gameState->border_symbol;
		gameState->map[i][SCREEN_WIDTH-1] = gameState->border_symbol;
	}
		
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			mvprintw(i, j, "%c", gameState->map[i][j]);
		}
	}
	attroff(COLOR_PAIR(1));
	// META
	attron(COLOR_PAIR(5));
	for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
		gameState->map[1][i] = '_';
		mvprintw(1, i, "%c", gameState->map[1][i]);
	}
	attroff(COLOR_PAIR(5));

	attron(COLOR_PAIR(gameState->frog_color));
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			mvaddch(gameState->frog_y + j, gameState->frog_x + i, gameState->frog_symbol);
		}
	}
	attroff(COLOR_PAIR(gameState->frog_color));

	for (int i = 0; i < gameState->max_cars; i++) {
		attron(COLOR_PAIR(cars[i].color));
		mvaddch(cars[i].car_y, cars[i].car_x, cars[i].symbol);
		attroff(COLOR_PAIR(cars[i].color));
	}
}

void setFrog(int y, int x, struct GameState* gameState) {
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			gameState->map[y + j][x + i] = gameState->frog_symbol;
		}
	}
}

void initColors() {
	if (has_colors()) {
		start_color();
		init_pair(1, COLOR_YELLOW, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_BLACK);
		init_pair(3, COLOR_RED, COLOR_BLACK);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
		init_pair(5, COLOR_RED, COLOR_RED);
	}
}

//FUNCTION TOO LONG
void inputDetect(struct GameState* gameState) {
	gameState->move = getch();
	switch (gameState->move) {
	case 'w':
	case 'W':
	case KEY_UP:
		if (gameState->frog_y > 1) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_y-=2;
		}
		break;
	case 's':
	case 'S':
	case KEY_DOWN:
		if (gameState->frog_y < SCREEN_HEIGHT - FROG_HEIGHT - 1) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_y+=2;
		}
		break;
	case 'a':
	case 'A':
	case KEY_LEFT:
		if (gameState->frog_x > 2) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x-=2;
		}
		// think if this loop is needed
		if (gameState->frog_x <= 2 && gameState->frog_x > 1) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x -= 1;
		}
		break;
	case 'd':
	case 'D':
	case KEY_RIGHT:
		if (gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH - 1) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x += 2;
		}
		if (gameState->frog_x >= SCREEN_WIDTH - FROG_WIDTH - 2 && gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH -1) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x += 1;
		}
		break;
	case 'q':
	case 'Q':
		gameState->quit = 0;
		break;
	default:
		break;
	}
	refresh();
	napms(0);
}

void ifScored(struct GameState* gameState) {
	if (gameState->frog_y == 1) {
		gameState->points++;
		gameState->frog_y = SCREEN_HEIGHT - 2;
		gameState->frog_x = SCREEN_WIDTH / 2;
	}
}

void initializeCars(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (!cars[i].initialized) {
			int speed_choice = (rand() % 3) + 1;
			int type_choice = (rand() % 2) + 1;
			cars[i].car_y = (i * 2) + 3;
			if (i % 2) {
				cars[i].car_x = 2;
				cars[i].direction = 'r';
			}
			else if (!(i%2)) {
				cars[i].car_x = SCREEN_WIDTH - 2;
				cars[i].direction = 'l';
			}
			if (type_choice == 1) {
				cars[i].type = 'w';
				cars[i].color = gameState->wrapping_car_color;
			}
			else if (type_choice == 2) {
				cars[i].type = 'b';
				cars[i].color = gameState->bouncing_car_color;
			}
			cars[i].speed = speed_choice;
			cars[i].symbol = gameState->car_symbol;
			cars[i].initialized = 1;
		}
	}
}

void bounceCar(struct Car* cars, int i) {
	if (cars[i].type == 'b') {
		if (cars[i].car_x - cars[i].speed * CAR_SPEED_VAR <= 0) {
			cars[i].direction = 'r';
		}
		else if (cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH) {
			cars[i].direction = 'l';
		}
	}
}

void wrapCar(struct Car* cars, int i) {
	if (cars[i].type == 'w') {
		if (cars[i].car_x - cars[i].speed <= 0 && cars[i].direction == 'l') {
			cars[i].car_x = SCREEN_WIDTH;
		}
		else if (cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH && cars[i].direction == 'r') {
			cars[i].car_x = 0;
		}
	}
}

void moveCars(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		bounceCar(cars, i);
		wrapCar(cars, i);
		if (cars[i].direction == 'r') {
			cars[i].car_x += cars[i].speed * CAR_SPEED_VAR;
		}
		else if (cars[i].direction == 'l') {
			cars[i].car_x -= cars[i].speed * CAR_SPEED_VAR;
		}
	}
}

void collisionDetect(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (gameState->frog_x <= cars[i].car_x + gameState->radius && gameState->frog_x >= cars[i].car_x - gameState->radius && gameState->frog_y == cars[i].car_y) {
			gameState->collisionDetected++;
			gameState->frog_y = SCREEN_HEIGHT - 2;
			gameState->frog_x = SCREEN_WIDTH / 2;
		}
	}
}

int readConfigFile(const char* filename, struct GameState* gameState, struct Car* cars) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror("Unable to open config file");
		exit(EXIT_FAILURE);
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		char key[128], value[128];
		// Split the line into key and value using '=' as delimiter
		if (sscanf(line, "%[^=]=%s", key, value) == 2) {
			// Match the key and assign the value to the appropriate field

			if (strcmp(key, "frog_color") == 0) {
				gameState->frog_color = atoi(value);
			}
			else if (strcmp(key, "bouncing_car_color") == 0) {
				gameState->bouncing_car_color = atoi(value);
			}
			else if (strcmp(key, "wrapping_car_color") == 0) {
				gameState->wrapping_car_color = atoi(value);
			}

			else if (strcmp(key, "frog_symbol") == 0) {
				gameState->frog_symbol = value[0]; // Take the first character
			}
			else if (strcmp(key, "car_symbol") == 0) {
				gameState->car_symbol = value[0]; // Take the first character
			}

			else if (strcmp(key, "max_cars") == 0) {
				gameState->max_cars = atoi(value);
			}
			else if (strcmp(key, "car_speed_variable") == 0) {
				gameState->car_speed_variable = atoi(value);
			}


			else if (strcmp(key, "border_symbol") == 0) {
				gameState->border_symbol = value[0]; // Take the first character
			}
			else if (strcmp(key, "lane_separator") == 0) {
				gameState->lane_separator = value[0]; // Take the first character
			}

			else if (strcmp(key, "max_time") == 0) {
				gameState->max_time = atoi(value);
			}

		}
	}

	fclose(file);
}

int main() {
	struct GameState gameState;
	struct Car* cars = (struct Car*)malloc(gameState.max_cars * sizeof(struct Car));
	srand(time(NULL));
	gameState.quit = 1;
	initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	initColors();
	readConfigFile("config.txt", &gameState, cars);
	while (gameState.quit && gameState.timer < gameState.max_time) {
		clear();
		setFrog(gameState.frog_y, gameState.frog_x, &gameState);
		drawMap(&gameState, cars);
		mvprintw(0, 2, "Time: %d seconds", gameState.timer);
		mvprintw(0, SCREEN_WIDTH - 17, "Points: %d", gameState.points);
		refresh();
		napms(1000);
		gameState.timer++;
		inputDetect(&gameState);
		ifScored(&gameState);
		initializeCars(cars, &gameState);
		moveCars(cars, &gameState);
		collisionDetect(cars, &gameState);
	}
	free(cars);
	endwin();
	return 0;

}
