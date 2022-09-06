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

void fill(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			board[i][j] = c;
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

// Game loop

int main() {
	char board[BOARD_SIZE][BOARD_SIZE];
	fill(board, EMPTY);
	
	while (true) {
		int row, col;
		scanf("%i%i", &row, &col);

		struct Result res = set(board, row, col, P1);
		if (!res.ok) {
			puts(res.err_msg);
			continue;
		}

		draw(board);
	}
}
