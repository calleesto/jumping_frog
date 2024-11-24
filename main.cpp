#include <curses.h>
#define SCREEN_WIDTH 41
#define SCREEN_HEIGHT 15
#define FROG_HEIGHT 1
#define FROG_WIDTH 1
#define FROG_SYMBOL 'O'
#define FROG_COLOR 4 // 1 - YELLOW, 2 - BLUE, 3 - RED, 4 - GREEN
#define BORDER_SYMBOL '#'
#define LANE_SEPARATOR '-'
#define MAX_TIME	60// in seconds

struct GameState {
	int quit;
	int frog_y = SCREEN_HEIGHT - 2;
	int frog_x = SCREEN_WIDTH/2;
	int move;
	char map[SCREEN_HEIGHT][SCREEN_WIDTH] = {' '};
	int timer = 0;
	int points = 0;
};

void drawMap(struct GameState* gameState) {
	attron(COLOR_PAIR(1));
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 2; j < SCREEN_HEIGHT; j++) {
			if (j % 2 == 0) {
				gameState->map[j][i] = LANE_SEPARATOR;
			}
		}
		gameState->map[0][i] = BORDER_SYMBOL;
		gameState->map[SCREEN_HEIGHT-1][i] = BORDER_SYMBOL;
	}
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		gameState->map[i][0] = BORDER_SYMBOL;
		gameState->map[i][SCREEN_WIDTH-1] = BORDER_SYMBOL;
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

	attron(COLOR_PAIR(FROG_COLOR));
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			mvaddch(gameState->frog_y + j, gameState->frog_x + i, FROG_SYMBOL);
		}
	}
	attroff(COLOR_PAIR(FROG_COLOR));
}

void setFrog(int y, int x, struct GameState* gameState) {
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			gameState->map[y + j][x + i] = FROG_SYMBOL;
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
		break;
		break;
	default:
		break;
	}
	refresh();
	// check whats the best time break for each jump
	napms(0);
}

void ifScored(struct GameState* gameState) {
	if (gameState->frog_y == 1) {
		gameState->points++;
		gameState->frog_y = SCREEN_HEIGHT - 2;
		gameState->frog_x = SCREEN_WIDTH / 2;
	}
}

int main() {
	struct GameState gameState;
	gameState.quit = 1;
	initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	initColors();
	while (gameState.quit && gameState.timer < MAX_TIME) {
		clear();
		setFrog(gameState.frog_y, gameState.frog_x, &gameState);
		drawMap(&gameState);
		mvprintw(0, 2, "Time: %d seconds", gameState.timer);
		mvprintw(0, SCREEN_WIDTH - 17, "Points: %d", gameState.points);
		refresh();
		napms(1000);
		gameState.timer++;
		inputDetect(&gameState);
		ifScored(&gameState);
	}
	endwin();
	return 0;

}



