#include <curses.h>
#define WIDTH 40
#define HEIGHT 20
#define PADDLE_HEIGHT 4
void draw_border() {
	attron(COLOR_PAIR(1));
	for (int x = 0; x < WIDTH; x++) mvaddch(0, x, '#'), mvaddch(HEIGHT - 1, x, '#');
	for (int y = 0; y < HEIGHT; y++) mvaddch(y, 0, '#'), mvaddch(y, WIDTH - 1, '#');
	attroff(COLOR_PAIR(1));
}
void draw_paddle(int y, int x, int color) {
	attron(COLOR_PAIR(color));
	for (int i = 0; i < PADDLE_HEIGHT; i++) mvaddch(y + i, x, '|');
	attroff(COLOR_PAIR(color));
}
int main() {
	initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
	if (has_colors()) {
		start_color();
		init_pair(1, COLOR_YELLOW, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_BLACK);
		init_pair(3, COLOR_RED, COLOR_BLACK);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
	}
	int paddle1_y = (HEIGHT - PADDLE_HEIGHT) / 2, paddle1_x = 1;
	int paddle2_y = (HEIGHT - PADDLE_HEIGHT) / 2, paddle2_x = WIDTH - 2;
	int ball_y = HEIGHT / 2, ball_x = WIDTH / 2;
	int ball_dir_y = 1, ball_dir_x = 1, ch;
	while (1) {
		clear(); draw_border();
		draw_paddle(paddle1_y, paddle1_x, 2);
		draw_paddle(paddle2_y, paddle2_x, 3);
		attron(COLOR_PAIR(4));
		mvaddch(ball_y, ball_x, 'O');
		attroff(COLOR_PAIR(4));
		ball_y += ball_dir_y;
		ball_x += ball_dir_x;
		if (ball_y <= 1 || ball_y >= HEIGHT - 2) ball_dir_y *= -1;
		if ((ball_x == paddle1_x + 1 && ball_y >= paddle1_y && ball_y < paddle1_y + PADDLE_HEIGHT) ||
			(ball_x == paddle2_x - 1 && ball_y >= paddle2_y && ball_y < paddle2_y + PADDLE_HEIGHT)) ball_dir_x *= -1;
		if (ball_x <= 1 || ball_x >= WIDTH - 2) ball_y = HEIGHT / 2, ball_x = WIDTH / 2, ball_dir_x *= -1;
		ch = getch();
		if (ch == 'q') break;
		if (ch == 'w' && paddle1_y > 1) paddle1_y--;
		if (ch == 's' && paddle1_y < HEIGHT - PADDLE_HEIGHT - 1) paddle1_y++;
		if (ch == KEY_UP && paddle2_y > 1) paddle2_y--;
		if (ch == KEY_DOWN && paddle2_y < HEIGHT - PADDLE_HEIGHT - 1) paddle2_y++;
		refresh();
		napms(75);
	}
	endwin();
	return 0;
}