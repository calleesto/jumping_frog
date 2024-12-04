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
#define MAX_OBSTACLES 10
#define MAX_OBSTACLE_LENGHT 10
#define OBSTACLE_SYMBOL '@'
#define SPEED_CHANGE_INTERVAL 5

#define MAX_CARS 5



struct GameState {
	int quit;
	int frog_y = SCREEN_HEIGHT - 2;
	int frog_x = SCREEN_WIDTH / 2;
	int frog_color;
	int move;
	char map[SCREEN_HEIGHT][SCREEN_WIDTH] = { ' ' };
	char obstacleMap[SCREEN_HEIGHT][SCREEN_WIDTH] = { ' ' };
	int timer = 0;
	int points = 0;
	int radius = CAR_SPEED_VAR + 1;
	int collisionDetected = 0;
	char frog_symbol;
	char car_symbol;
	int car_speed_variable;
	int passive_car_color;
	int aggressive_car_color;
	int friendly_car_color;
	int max_cars;
	int border_symbol;
	int lane_separator;
	int max_time;
	int number_of_bounces;
	int number_of_wraps;
	bool moveLeft = true;
	bool moveRight = true;
	bool moveUp = true;
	bool moveDown = true;
	bool obstaclesSet = false;
};

struct Car {
	int car_x;
	int car_y;
	char direction;
	int speed;
	char type; // 'w' - wrapping, 'b' - bouncing, 'd' - disappearing
	char interaction; // 'p' - passive, 'a' - aggressive
	char symbol;
	int color;
	int initialized = 0;// 1 - initialized, 0 - not initialized
	int iters;
	bool stopCar = false;
};


void setObstacles(struct GameState* gameState) {
	int rand_x;
	int rand_y;
	int n;
	if (gameState->obstaclesSet == false) {
		for (int i = 0; i < MAX_OBSTACLES; i++) {
		Repeat:
			rand_x = (rand() % SCREEN_WIDTH) + 1;
			rand_y = (rand() % SCREEN_HEIGHT);

			while ((rand_y % 2 != 0) || (rand_y == 0) || (rand_y == SCREEN_HEIGHT - 1)) {
				rand_y = (rand() % SCREEN_HEIGHT);
			}

			n = (rand() % 10) + 1;
			if (n < 3) {
				n = 3;
			}
			for (int i = 0; i < n; i++) {
				if (gameState->obstacleMap[rand_y][rand_x + i] == OBSTACLE_SYMBOL) {
					goto Repeat;
				}
				gameState->obstacleMap[rand_y][rand_x + i] = OBSTACLE_SYMBOL;
			}
		}
		for (int i = 0; i < SCREEN_HEIGHT; i++) {
			gameState->obstacleMap[i][0] = ' ';
			gameState->obstacleMap[i][SCREEN_WIDTH - 1] = ' ';
		}
		for (int j = 1; j < SCREEN_HEIGHT; j += 2) {
			for (int i = 0; i < SCREEN_WIDTH; i++) {
				gameState->obstacleMap[j][i] = ' ';

			}
		}
		gameState->obstaclesSet = true;
	}
}


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
		gameState->map[SCREEN_HEIGHT - 1][i] = gameState->border_symbol;
	}
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		gameState->map[i][0] = gameState->border_symbol;
		gameState->map[i][SCREEN_WIDTH - 1] = gameState->border_symbol;
	}
	setObstacles(gameState);
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			if (gameState->obstacleMap[i][j] == OBSTACLE_SYMBOL) {
				gameState->map[i][j] = OBSTACLE_SYMBOL;
			}
		}
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
		init_pair(6, COLOR_WHITE, COLOR_BLACK);
	}
}

//FUNCTION TOO LONG
void inputDetect(struct GameState* gameState) {
	gameState->move = getch();
	switch (gameState->move) {
	case 'w':
	case 'W':
	case KEY_UP:
		if (gameState->frog_y > 1 && gameState->moveUp) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_y -= 1;
		}
		break;
	case 's':
	case 'S':
	case KEY_DOWN:
		if (gameState->frog_y < SCREEN_HEIGHT - FROG_HEIGHT - 1 && gameState->moveDown) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_y += 1;
		}
		break;
	case 'a':
	case 'A':
	case KEY_LEFT:
		if (gameState->frog_x > 2 && gameState->moveLeft) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x -= 1;
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
		if (gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH - 1 && gameState->moveRight) {
			gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
			gameState->frog_x += 1;
		}
		if (gameState->frog_x >= SCREEN_WIDTH - FROG_WIDTH - 2 && gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH - 1) {
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

void typeChoiceCase(int choice, int n, struct Car* cars, char symbol, int i) {
	if (choice == n) {
		cars[i].type = symbol;
	}
}

void interactionChoiceCase(int choice, int n, struct Car* cars, char symbol, int i, int color) {
	if (choice == n) {
		cars[i].interaction = symbol;
		cars[i].color = color;
	}
}

void initializeCars(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (cars[i].initialized == 0) {

			int speed_choice = (rand() % 3) + 1;
			int type_choice = (rand() % 3) + 1;
			int interaction_choice = (rand() % 3) + 1;

			cars[i].car_y = (i * 2) + 3;

			if (i % 2) {
				cars[i].car_x = 2;
				cars[i].direction = 'r';
			}
			else if (!(i % 2)) {
				cars[i].car_x = SCREEN_WIDTH - 2;
				cars[i].direction = 'l';
			}

			typeChoiceCase(type_choice, 1, cars, 'w', i);
			typeChoiceCase(type_choice, 2, cars, 'b', i);
			typeChoiceCase(type_choice, 3, cars, 'd', i);

			interactionChoiceCase(interaction_choice, 1, cars, 'p', i, gameState->passive_car_color);
			interactionChoiceCase(interaction_choice, 2, cars, 'a', i, gameState->aggressive_car_color);
			interactionChoiceCase(interaction_choice, 3, cars, 'f', i, gameState->friendly_car_color);

			cars[i].speed = speed_choice;
			cars[i].symbol = gameState->car_symbol;
			cars[i].iters = 0;
			cars[i].initialized = 1;
		}
	}
}

void changeCarAfterNumOfIters(int iterations, struct Car* cars, int i) {
	if (cars[i].iters > iterations) {
		cars[i].iters = 0;
		cars[i].initialized = 0;
	}
}

void bounceCar(struct Car* cars, int i, struct GameState* gameState) {
	if (cars[i].type == 'b') {
		if (cars[i].car_x - cars[i].speed * CAR_SPEED_VAR <= 0) {
			cars[i].iters++;
			changeCarAfterNumOfIters(gameState->number_of_bounces, cars, i);
			cars[i].direction = 'r';
		}
		else if (cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH) {
			cars[i].iters++;
			changeCarAfterNumOfIters(gameState->number_of_bounces, cars, i);
			cars[i].direction = 'l';
		}
	}
}

void wrapCar(struct Car* cars, int i, struct GameState* gameState) {
	if (cars[i].type == 'w') {
		if (cars[i].car_x - cars[i].speed * CAR_SPEED_VAR <= 0 && cars[i].direction == 'l') {
			cars[i].iters++;
			changeCarAfterNumOfIters(gameState->number_of_wraps, cars, i);
			cars[i].car_x = SCREEN_WIDTH;
		}
		else if (cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH && cars[i].direction == 'r') {
			cars[i].iters++;
			changeCarAfterNumOfIters(gameState->number_of_wraps, cars, i);
			cars[i].car_x = 0;
		}
	}
}

void disappearCar(struct Car* cars, int i) {
	if (cars[i].type == 'd') {
		if (cars[i].car_x - cars[i].speed * CAR_SPEED_VAR <= 0 && cars[i].direction == 'l' || cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH && cars[i].direction == 'r') {
			cars[i].initialized = 0;
		}
	}
}

void moveCars(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (cars[i].stopCar == false) {
			bounceCar(cars, i, gameState);
			wrapCar(cars, i, gameState);
			disappearCar(cars, i);
			if (cars[i].direction == 'r') {
				cars[i].car_x += cars[i].speed * CAR_SPEED_VAR;
			}
			else if (cars[i].direction == 'l') {
				cars[i].car_x -= cars[i].speed * CAR_SPEED_VAR;
			}
		}
	}
}

void collisionDetect(struct Car* cars, struct GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (gameState->frog_x <= cars[i].car_x + gameState->radius && gameState->frog_x >= cars[i].car_x - gameState->radius && gameState->frog_y == cars[i].car_y) {
			if (cars[i].interaction == 'a') {
				gameState->collisionDetected++;
				gameState->frog_y = SCREEN_HEIGHT - 2;
				gameState->frog_x = SCREEN_WIDTH / 2;
			}
			else if (cars[i].interaction == 'p') {
				if (cars[i].direction == 'l' && cars[i].car_x > gameState->frog_x || cars[i].direction == 'r' && cars[i].car_x < gameState->frog_x) {
					cars[i].stopCar = true;
				}
			}
		}
		else {
			cars[i].stopCar = false;
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
			else if (strcmp(key, "passive_car_color") == 0) {
				gameState->passive_car_color = atoi(value);
			}
			else if (strcmp(key, "aggressive_car_color") == 0) {
				gameState->aggressive_car_color = atoi(value);
			}
			else if (strcmp(key, "friendly_car_color") == 0) {
				gameState->friendly_car_color = atoi(value);
			}
			else if (strcmp(key, "number_of_bounces") == 0) {
				gameState->number_of_bounces = atoi(value);
			}
			else if (strcmp(key, "number_of_wraps") == 0) {
				gameState->number_of_wraps = atoi(value);
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

void checkForObstacle(struct GameState* gameState) {
	if (gameState->map[gameState->frog_y - 1][gameState->frog_x] == OBSTACLE_SYMBOL) {
		gameState->moveUp = false;
	}
	if (gameState->map[gameState->frog_y + 1][gameState->frog_x] == OBSTACLE_SYMBOL) {
		gameState->moveDown = false;
	}
	if (gameState->map[gameState->frog_y][gameState->frog_x - 1] == OBSTACLE_SYMBOL) {
		gameState->moveLeft = false;
	}
	if (gameState->map[gameState->frog_y][gameState->frog_x + 1] == OBSTACLE_SYMBOL) {
		gameState->moveRight = false;
	}
}

void setAllMovementTrue(struct GameState* gameState) {
	gameState->moveLeft = true;
	gameState->moveRight = true;
	gameState->moveUp = true;
	gameState->moveDown = true;
}

void changeOfSpeed(struct GameState* gameState, struct Car* cars) {
	if ((gameState->timer % SPEED_CHANGE_INTERVAL) == 0 && gameState->timer != 0) {
		for (int i = 0; i < gameState->max_cars; i++) {
			int speed_choice = (rand() % 3) + 1;
			cars[i].speed = speed_choice;
		}
	}
}

int main() {
	struct GameState gameState;
	struct Car cars[MAX_CARS];
	//struct Car* cars = (struct Car*)malloc(gameState.max_cars * sizeof(struct Car));
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
		setAllMovementTrue(&gameState);
		checkForObstacle(&gameState);
		inputDetect(&gameState);
		ifScored(&gameState);
		initializeCars(cars, &gameState);
		changeOfSpeed(&gameState, cars);
		moveCars(cars, &gameState);
		collisionDetect(cars, &gameState);
	}
	free(cars);
	endwin();
	return 0;

}