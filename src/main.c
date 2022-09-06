#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 3
#define EMPTY ' '
#define P1 'X'
#define P2 'O'

struct Result {
	int ok;
	char err_msg[50];
};

struct SomePosition {
	int some;
	int row;
	int col;
};

void fill(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			board[i][j] = c;
}

struct SomePosition find_first_in_row(char board[BOARD_SIZE][BOARD_SIZE], int row, char c) {
	struct SomePosition res;

	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[row][i] == c) {
			res.some = 1;
			res.row = row;
			res.col = i;

			break;
		}

	return res;
}

struct SomePosition find_first_in_col(char board[BOARD_SIZE][BOARD_SIZE], int col, char c) {
	struct SomePosition res;

	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[i][col] == c) {
			res.some = 1;
			res.row = i;
			res.col = col;

			break;
		}

	return res;
}

struct SomePosition find_first_vertically(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	struct SomePosition res;

	// Top left to bottom right.
	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[i][i] == c) {
			res.some = 1;
			res.row = i;
			res.col = i;

			return res;
		}

	// Top right to bottom left.
	for (int i = 0; i < BOARD_SIZE; i++) {
		int j = (BOARD_SIZE - 1) - i;

		if (board[i][j] == c) {
			res.some = 1;
			res.row = i;
			res.col = j;

			return res;
		}
	}
}

// Board

void draw(char board[BOARD_SIZE][BOARD_SIZE]) {
	printf("%c|%c|%c\n"
		   "%c|%c|%c\n"
		   "%c|%c|%c\n",
		   board[0][0], board[0][1], board[0][2],
		   board[1][0], board[1][1], board[1][2],
		   board[2][0], board[2][1], board[2][2]);
}

struct Result set(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char c) {
	struct Result res;

	if (board[row][col] == EMPTY) {
		board[row][col] = c;

		res.ok = 1;
		strcpy(res.err_msg, "");
	} else {
		res.ok = 0;
		strcpy(res.err_msg, "Espaco ocupado");
	}

	return res;
}

// Bot

void play_first_pos(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) {
			struct Result res = set(board, i, j, P2);

			if (res.ok)
				return;
		}
}

void play(char board[BOARD_SIZE][BOARD_SIZE]) {
	play_first_pos(board);
}

// Game loop

int main() {
	char board[BOARD_SIZE][BOARD_SIZE];
	fill(board, EMPTY);
	
	while (true) {
		// User play

		int row, col;
		scanf("%i%i", &row, &col);

		struct Result res = set(board, row, col, P1);
		if (!res.ok) {
			puts(res.err_msg);
			continue;
		}

		draw(board);

		// Bot play

		play(board);
		draw(board);
	}
}
