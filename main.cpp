#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define SCREEN_WIDTH			41
#define SCREEN_HEIGHT			15
#define FROG_HEIGHT				1
#define FROG_WIDTH				1
#define CAR_SPEED_VAR			0.05
#define MAX_OBSTACLES			5
#define MAX_OBSTACLE_LENGHT		5
#define OBSTACLE_SYMBOL			'@'
#define SPEED_CHANGE_INTERVAL	5
#define MAX_CARS				5
#define MAP_ELEMENTS_COLOR		1
#define FINISH_LANE_COLOR		5
#define TIMER_ADDITION			0.015
#define DELAY_AFTER_JUMP		0.001 // 0.3 for visible delay but still playable --- no delay seems more comfortabel though

/*
todo:
1. header files 

2. fix all car bugs 

4. fix folder hierarchy git repo issue
	- just add another repo with two folder down
	- use both for presentation the new one to show off project
	- the old one to show off commit history
	- call the new one "jumping_frog_finished"
*/


typedef struct GameState {
	int quit = 1;
	int frog_y = SCREEN_HEIGHT - 2;
	int frog_x = SCREEN_WIDTH / 2;
	int frog_color = 0;
	int move = 0;
	char map[SCREEN_HEIGHT][SCREEN_WIDTH] = { ' ' };
	char obstacleMap[SCREEN_HEIGHT][SCREEN_WIDTH] = { ' ' };
	float timer = 0;
	int points = 0;
	int radius = CAR_SPEED_VAR + 1;
	int collisionDetected = 0;
	char frog_symbol = ' ';
	char car_symbol = ' ';
	int car_speed_variable = 0;
	int passive_car_color = 0;
	int aggressive_car_color = 0;
	int friendly_car_color = 0;
	int max_cars = 0;
	char border_symbol = ' ';
	char lane_separator = ' ';
	int max_time = 0;
	int number_of_bounces = 0;
	int number_of_wraps = 0;
	bool moveLeft = true;
	bool moveRight = true;
	bool moveUp = true;
	bool moveDown = true;
	bool obstaclesSet = false;
	bool frogRide = false;
	int highscore = 0;
	bool wDetected = false;
	bool aDetected = false;
	bool sDetected = false;
	bool dDetected = false;
	bool inputDetected = false;
	float save_timer;
} GameState;

typedef struct Car{
	float car_x = 0.0;
	int car_y = 0;
	char direction = ' ';
	float speed = 0.0;
	char type = ' '; // 'w' - wrapping, 'b' - bouncing, 'd' - disappearing
	char interaction = ' '; // 'p' - passive, 'a' - aggressive
	char symbol = ' ';
	int color = 0;
	int initialized = 0;// 1 - initialized, 0 - not initialized
	int iters = 0;
	bool stopCar = false;
}Car;

void setObstacles(GameState* gameState);
void setBordersAndSeparators(GameState* gameState);
void obstaclesToArray(GameState* gameState);
void printMapArray(GameState* gameState);
void printFinishLane(GameState* gameState);
void printFrog(GameState* gameState);
void printCars(GameState* gameState, Car* cars);
void printGameInfo(GameState* gameState);
void setAndPrintVisuals(GameState* gameState, Car* cars);
void initColors();
int frogRideOff(GameState* gameState);
void upCase(GameState* gameState);
void downCase(GameState* gameState);
void leftCase(GameState* gameState);
void rightCase(GameState* gameState);
void inputDetect(GameState* gameState);
void ifScored(GameState* gameState);
void typeChoiceCase(int choice, int n, Car* cars, char symbol, int i);
void interactionChoiceCase(int choice, int n, Car* cars, char symbol, int i, int color);
void carDirection(int i, Car* cars);
void initializeCars(Car* cars, GameState* gameState);
void changeCarAfterNumOfIters(int iterations, Car* cars, int i);
void bounceCar(Car* cars, int i, GameState* gameState);
void wrapCar(Car* cars, int i, GameState* gameState);
void disappearCar(Car* cars, int i, GameState* gameState);
void moveCars(Car* cars, GameState* gameState);
void aggressiveCase(int i, Car* cars, GameState* gameState);
void passiveCase(int i, Car* cars, GameState* gameState);
void friendlyCase(int i, Car* cars, GameState* gameState);
void collisionDetect(Car* cars, GameState* gameState);
void extractValue(char key[128], char value[128], const char* var_name, int* var_value_int, char* var_value_char);
void extractValues(char key[128], char value[128], GameState* gameState);
void readConfigFile(const char* filename, GameState* gameState);
void checkForObstacle(GameState* gameState);
void setAllMovementTrue(GameState* gameState);
void changeOfSpeed(GameState* gameState, Car* cars);
void setNewHighscore(GameState* gameState);
void resetVariables(GameState* gameState, Car* cars);
void resetGame(GameState* gameState, Car* cars);
void letAnotherDetect(GameState* gameState);

void setObstacles(GameState* gameState) {
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

void setBordersAndSeparators(GameState* gameState) {
	//put these objects on the map array:
	//-top border
	//-bottom border
	//-lane separators
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 2; j < SCREEN_HEIGHT; j++) {
			if (j % 2 == 0) {
				gameState->map[j][i] = gameState->lane_separator;
			}
		}
		gameState->map[0][i] = gameState->border_symbol;
		gameState->map[SCREEN_HEIGHT - 1][i] = gameState->border_symbol;
	}
	//put side borders on the map array
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		gameState->map[i][0] = gameState->border_symbol;
		gameState->map[i][SCREEN_WIDTH - 1] = gameState->border_symbol;
	}
}

void obstaclesToArray(GameState* gameState) {
	//set obstacles and put them on the map array
	setObstacles(gameState);
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			if (gameState->obstacleMap[i][j] == OBSTACLE_SYMBOL) {
				gameState->map[i][j] = OBSTACLE_SYMBOL;
			}
		}
	}
}

void printMapArray(GameState* gameState) {
	//chooses color nr 1 which is yellow
	attron(COLOR_PAIR(MAP_ELEMENTS_COLOR));
	//print map array
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			mvprintw(i, j, "%c", gameState->map[i][j]);
		}
	}
	//color off
	attroff(COLOR_PAIR(MAP_ELEMENTS_COLOR));
}

void printFinishLane(GameState* gameState) {
	//finish lane background color = red
	attron(COLOR_PAIR(FINISH_LANE_COLOR));
	for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
		gameState->map[1][i] = '_';
		mvprintw(1, i, "%c", gameState->map[1][i]);
	}
	attroff(COLOR_PAIR(FINISH_LANE_COLOR));
}

void printFrog(GameState* gameState) {
	//choose color for the frog
	attron(COLOR_PAIR(gameState->frog_color));
	//print frog
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			mvaddch(gameState->frog_y + j, gameState->frog_x + i, gameState->frog_symbol);
		}
	}
	attroff(COLOR_PAIR(gameState->frog_color));
}

int isAtWholeNumber(float timer) {
	if (timer - floor(timer) >= 0.0 && timer - floor(timer) <= TIMER_ADDITION) {
		return true;
	}
	return false;
}

void printCars(GameState* gameState, Car* cars) {
	//print cars
	for (int i = 0; i < gameState->max_cars; i++) {
		//choose color for cars
		attron(COLOR_PAIR(cars[i].color));
		if (isAtWholeNumber(gameState->timer)) {
			mvaddch(cars[i].car_y, (int)cars[i].car_x, cars[i].symbol);
		}
		else if (!isAtWholeNumber(gameState->timer)) {
			mvaddch(cars[i].car_y, floor(cars[i].car_x), cars[i].symbol);
		}
		attroff(COLOR_PAIR(cars[i].color));
	}
}

void printGameInfo(GameState* gameState) {
	mvprintw(0, 1, "Time: [%.2f]s", gameState->timer);
	mvprintw(0, SCREEN_WIDTH - 20, "Points: [%d]", gameState->points);
	mvprintw(0, SCREEN_WIDTH - 7, "HS: [%d]", gameState->highscore);
}

void setAndPrintVisuals(GameState* gameState, Car* cars) {
	setBordersAndSeparators(gameState);
	obstaclesToArray(gameState);
	printMapArray(gameState);
	printFinishLane(gameState);
	printFrog(gameState);
	printCars(gameState, cars);
	printGameInfo(gameState);
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

int frogRideOff(GameState* gameState) {
	if (gameState->map[gameState->frog_y][gameState->frog_x] != OBSTACLE_SYMBOL && gameState->frogRide == true) {
		gameState->frogRide = false;
		return true;
	}
	return false;
}

void upCase(GameState* gameState) {
	if (gameState->frog_y > 1 && gameState->moveUp) {
		gameState->frog_y -= 1;
		gameState->inputDetected = true;
		gameState->save_timer = gameState->timer;
	}
}

void downCase(GameState* gameState) {
	if (gameState->frog_y < SCREEN_HEIGHT - FROG_HEIGHT - 1 && gameState->moveDown) {
		gameState->frog_y += 1;
		gameState->inputDetected = true;
		gameState->save_timer = gameState->timer;
	}
}

void leftCase(GameState* gameState) {
	if (gameState->frog_x > 2 && gameState->moveLeft) {
		gameState->frog_x -= 1;
		gameState->inputDetected = true;
		gameState->save_timer = gameState->timer;
	}
}

void rightCase(GameState* gameState) {
	if (gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH - 1 && gameState->moveRight) {
		gameState->frog_x += 1;
		gameState->inputDetected = true;
		gameState->save_timer = gameState->timer;
	}
}

void inputDetect(GameState* gameState) {
	gameState->move = getch();
	if (gameState->inputDetected == false) {
		switch (gameState->move) {
		case 'w':
			if (frogRideOff(gameState)) {
				break;
			}
			upCase(gameState);
			break;
		case 's':
			if (frogRideOff(gameState)) {
				break;
			}
			downCase(gameState);
			break;
		case 'a':
			if (frogRideOff(gameState)) {
				break;
			}
			leftCase(gameState);
			break;
		case 'd':
			if (frogRideOff(gameState)) {
				break;
			}
			rightCase(gameState);
			break;
		case 'q':
			gameState->quit = 0;
			break;
		}
	}
}

void letAnotherDetect(GameState* gameState) {
	if (gameState->timer - gameState->save_timer > DELAY_AFTER_JUMP) {
		gameState->inputDetected = false;
	}
}

void ifScored(GameState* gameState) {
	if (gameState->frog_y == 1) {
		gameState->points++;
		gameState->frog_y = SCREEN_HEIGHT - 2;
		gameState->frog_x = SCREEN_WIDTH / 2;
	}
}

void typeChoiceCase(int choice, int n, Car* cars, char symbol, int i) {
	if (choice == n) {
		cars[i].type = symbol;
	}
}

void interactionChoiceCase(int choice, int n, Car* cars, char symbol, int i, int color) {
	if (choice == n) {
		cars[i].interaction = symbol;
		cars[i].color = color;
	}
}

void carDirection(int i, Car* cars) {
	if (i % 2) {
		cars[i].car_x = 2;
		cars[i].direction = 'r';
	}
	else if (!(i % 2)) {
		cars[i].car_x = SCREEN_WIDTH - 2;
		cars[i].direction = 'l';
	}
}

void initializeCars(Car* cars, GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (cars[i].initialized == 0) {

			int speed_choice = (rand() % 3) + 1;
			int type_choice = (rand() % 3) + 1;
			int interaction_choice = (rand() % 3) + 1;

			cars[i].car_y = (i * 2) + 3;

			carDirection(i, cars);

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

void changeCarAfterNumOfIters(int iterations, Car* cars, int i) {
	if (cars[i].iters > iterations) {
		cars[i].iters = 0;
		cars[i].initialized = 0;
	}
}

void bounceCar(Car* cars, int i, GameState* gameState) {
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

void wrapCar(Car* cars, int i, GameState* gameState) {
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

void disappearCar(Car* cars, int i, GameState* gameState) {
	if (cars[i].type == 'd') {
		if (cars[i].car_x - cars[i].speed * CAR_SPEED_VAR <= 0 && cars[i].direction == 'l' || cars[i].car_x + cars[i].speed * CAR_SPEED_VAR >= SCREEN_WIDTH && cars[i].direction == 'r') {
			gameState->frogRide = false;
			gameState->frog_x += 5;
			cars[i].initialized = 0;
		}
	}
}

void moveCars(Car* cars, GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if (cars[i].stopCar == false) {
			bounceCar(cars, i, gameState);
			wrapCar(cars, i, gameState);
			disappearCar(cars, i, gameState);
			if (cars[i].direction == 'r') {
				cars[i].car_x += cars[i].speed * CAR_SPEED_VAR;
			}
			else if (cars[i].direction == 'l') {
				cars[i].car_x -= cars[i].speed * CAR_SPEED_VAR;
			}
		}
	}
}

void aggressiveCase(int i, Car* cars, GameState* gameState) {
	if (cars[i].interaction == 'a') {
		gameState->collisionDetected++;
		gameState->frog_y = SCREEN_HEIGHT - 2;
		gameState->frog_x = SCREEN_WIDTH / 2;
	}
}

void passiveCase(int i, Car* cars, GameState* gameState) {
	if (cars[i].interaction == 'p') {
		if (cars[i].direction == 'l' && cars[i].car_x > gameState->frog_x || cars[i].direction == 'r' && cars[i].car_x < gameState->frog_x) {
			cars[i].stopCar = true;
		}
	}
}

void friendlyCase(int i, Car* cars, GameState* gameState) {
	if ((cars[i].interaction == 'f' && cars[i].direction == 'l' && cars[i].car_x > gameState->frog_x || cars[i].direction == 'r' && cars[i].car_x < gameState->frog_x) || gameState->frogRide == true) {
		gameState->map[gameState->frog_y][gameState->frog_x] = ' ';
		gameState->frogRide = true;
		gameState->frog_x = cars[i].car_x;
		gameState->frog_y = cars[i].car_y - 1;
	}
}

void collisionDetect(Car* cars, GameState* gameState) {
	for (int i = 0; i < gameState->max_cars; i++) {
		if ((gameState->frog_x <= cars[i].car_x + gameState->radius && gameState->frog_x >= cars[i].car_x - gameState->radius && gameState->frog_y == cars[i].car_y) || gameState->frogRide == true) {
			aggressiveCase(i, cars, gameState);
			passiveCase(i, cars, gameState);
			friendlyCase(i, cars, gameState);
		}
		else {
			cars[i].stopCar = false;
		}
	}
}

void extractValue(char key[128], char value[128], const char* var_name, int* var_value_int, char* var_value_char) {
	if (strcmp(key, var_name) == 0) {
		if (var_value_int != NULL) {
			*var_value_int = atoi(value);
		}
		else if (var_value_char != NULL) {
			*var_value_char = value[0];
		}
	}
}

void extractValues(char key[128], char value[128], GameState* gameState) {
	extractValue(key, value, "frog_color", &gameState->frog_color, NULL);
	extractValue(key, value, "passive_car_color", &gameState->passive_car_color, NULL);
	extractValue(key, value, "aggressive_car_color", &gameState->aggressive_car_color, NULL);
	extractValue(key, value, "friendly_car_color", &gameState->friendly_car_color, NULL);
	extractValue(key, value, "number_of_bounces", &gameState->number_of_bounces, NULL);
	extractValue(key, value, "number_of_wraps", &gameState->number_of_wraps, NULL);
	extractValue(key, value, "frog_symbol", NULL, &gameState->frog_symbol);
	extractValue(key, value, "car_symbol", NULL, &gameState->car_symbol);
	extractValue(key, value, "max_cars", &gameState->max_cars, NULL);
	extractValue(key, value, "border_symbol", NULL, &gameState->border_symbol);
	extractValue(key, value, "lane_separator", NULL, &gameState->lane_separator);
	extractValue(key, value, "max_time", &gameState->max_time, NULL);
}

void readConfigFile(const char* filename, GameState* gameState) {
	FILE* file = fopen(filename, "r");

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		char key[128], value[128];
		if (sscanf(line, "%[^=]=%s", key, value) == 2) {
			extractValues(key, value, gameState);
		}
	}
	fclose(file);
}

void checkForObstacle(GameState* gameState) {
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

void setAllMovementTrue(GameState* gameState) {
	gameState->moveLeft = true;
	gameState->moveRight = true;
	gameState->moveUp = true;
	gameState->moveDown = true;
}

void changeOfSpeed(GameState* gameState, Car* cars) {
	if (((int)gameState->timer % SPEED_CHANGE_INTERVAL) == 0 && gameState->timer != 0) {
		for (int i = 0; i < gameState->max_cars; i++) {
			int speed_choice = (rand() % 3) + 1;
			cars[i].speed = speed_choice;
		}
	}
}

void setNewHighscore(GameState* gameState) {
	if (gameState->highscore < gameState->points) {
		gameState->highscore = gameState->points;
	}
}

void resetVariables(GameState* gameState, Car* cars) {
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			gameState->map[i][j] = ' ';
			gameState->obstacleMap[i][j] = ' ';
		}
	}
	gameState->frog_y = SCREEN_HEIGHT - 2;
	gameState->frog_x = SCREEN_WIDTH / 2;
	gameState->timer = 0;
	gameState->points = 0;
	gameState->radius = CAR_SPEED_VAR + 1;
	gameState->collisionDetected = 0;
	gameState->moveLeft = true;
	gameState->moveRight = true;
	gameState->moveUp = true;
	gameState->moveDown = true;
	gameState->obstaclesSet = false;
	gameState->frogRide = false;
	for (int i = 0; i < MAX_CARS; i++) {
		cars[i].initialized = 0;
	}
}

void resetGame(GameState* gameState,Car* cars) {
	if (gameState->timer > gameState->max_time) {
		setNewHighscore(gameState);
		resetVariables(gameState, cars);
	}
}

void slowDownFrog() {
	
}

int main() {
	GameState gameState;
	Car cars[MAX_CARS];
	srand(time(NULL));
	gameState.quit = 1;
	initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	initColors();
	readConfigFile("config.txt", &gameState);
	while (gameState.quit) {
		clear();
		setAndPrintVisuals(&gameState, cars);
		refresh();
		napms(1);
		gameState.timer += TIMER_ADDITION; //pretty accurate portrayal of real time
		setAllMovementTrue(&gameState);
		checkForObstacle(&gameState);
		inputDetect(&gameState);
		letAnotherDetect(&gameState);
		ifScored(&gameState);
		initializeCars(cars, &gameState);
		changeOfSpeed(&gameState, cars);
		moveCars(cars, &gameState);
		collisionDetect(cars, &gameState);
		resetGame(&gameState, cars);
	}
	free(cars);
	endwin();
	return 0;

}