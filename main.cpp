#include <curses.h>
#define SCREEN_WIDTH 41
#define SCREEN_HEIGHT 15
#define FROG_HEIGHT 1
#define FROG_WIDTH 1
#define FROG_SYMBOL 'O'
#define FROG_COLOR 4 // 1 - YELLOW, 2 - BLUE, 3 - RED, 4 - GREEN
#define BORDER_SYMBOL '#'
#define STREET_SEPARATOR '-'

struct GameState {
	int quit;
	int frog_y = SCREEN_HEIGHT - 2;
	int frog_x = SCREEN_WIDTH/2;
	int move;
	char map[SCREEN_HEIGHT][SCREEN_WIDTH] = {' '};
};

void drawMap(struct GameState* gameState) {
	attron(COLOR_PAIR(1));
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 2; j < SCREEN_HEIGHT; j++) {
			if (j % 2 == 0) {
				gameState->map[j][i] = STREET_SEPARATOR;
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
	/*
	for (int x = 0; x < SCREEN_WIDTH; x++) { mvaddch(SCREEN_HEIGHT - 1, x, BORDER_SYMBOL); }
	for (int x = 0; x < SCREEN_WIDTH; x++) { mvaddch(0, x, BORDER_SYMBOL); }
	for (int y = 0; y < SCREEN_HEIGHT; y++) { mvaddch(y, 0, BORDER_SYMBOL), mvaddch(y, SCREEN_WIDTH - 1, BORDER_SYMBOL); }
	*/
	attroff(COLOR_PAIR(1));
}

void draw_frog(int y, int x, int color) {
	attron(COLOR_PAIR(color));
	for (int j = 0; j < FROG_HEIGHT; j++) {
		for (int i = 0; i < FROG_WIDTH; i++) {
			mvaddch(y + j, x + i, FROG_SYMBOL);
		}
	}
	attroff(COLOR_PAIR(color));
}

void initColors() {
	if (has_colors()) {
		start_color();
		init_pair(1, COLOR_YELLOW, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_BLACK);
		init_pair(3, COLOR_RED, COLOR_BLACK);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
	}
}

void inputDetect(struct GameState* gameState) {

	gameState->move = getch();

	switch (gameState->move) {
	case 'w':
	case 'W':
	case KEY_UP:
		if (gameState->frog_y > 1) {
			gameState->frog_y-=2;
		}
		break;
	case 's':
	case 'S':
	case KEY_DOWN:
		if (gameState->frog_y < SCREEN_HEIGHT - FROG_HEIGHT - 1) {
			gameState->frog_y+=2;
		}
		break;
	case 'a':
	case 'A':
	case KEY_LEFT:
		if (gameState->frog_x > 2) {
			gameState->frog_x-=2;
		}
		// think if this loop is needed
		if (gameState->frog_x <= 2 && gameState->frog_x > 1) {
			gameState->frog_x -= 1;
		}
		break;
	case 'd':
	case 'D':
	case KEY_RIGHT:
		if (gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH - 1) {
			gameState->frog_x += 2;
		}
		if (gameState->frog_x >= SCREEN_WIDTH - FROG_WIDTH - 2 && gameState->frog_x < SCREEN_WIDTH - FROG_WIDTH -1) {
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

int main() {
	struct GameState gameState;
	gameState.quit = 1;
	initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	initColors();
	//int ball_y = SCREEN_HEIGHT / 2, ball_x = SCREEN_WIDTH / 2;
	//int ball_dir_y = 1, ball_dir_x = 1, ch;
	while (gameState.quit) {
		clear(); drawMap(&gameState);
		draw_frog(gameState.frog_y, gameState.frog_x, FROG_COLOR);
		//attron(COLOR_PAIR(4));
		//mvaddch(ball_y, ball_x, 'O');
		//attroff(COLOR_PAIR(4));
		//ball_y += ball_dir_y;
		//ball_x += ball_dir_x;
		//if (ball_y <= 1 || ball_y >= SCREEN_HEIGHT - 2) ball_dir_y *= -1;
		//if ((ball_x == frog_x + 1 && ball_y >= frog_y && ball_y < frog_y + PADDLE_SCREEN_HEIGHT) ||
			//(ball_x == paddle2_x - 1 && ball_y >= paddle2_y && ball_y < paddle2_y + PADDLE_SCREEN_HEIGHT)) ball_dir_x *= -1;
		//if (ball_x <= 1 || ball_x >= SCREEN_WIDTH - 2) ball_y = SCREEN_HEIGHT / 2, ball_x = SCREEN_WIDTH / 2, ball_dir_x *= -1;
		inputDetect(&gameState);

	}
	endwin();
	return 0;

}



