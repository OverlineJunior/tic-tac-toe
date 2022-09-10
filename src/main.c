/*
	TODO: Make corner return a *possible* position (struct for that).
	TODO: Function that returns any position other than corner.
	TODO: If position, first check if it can result in a triangle for the user;
		  if yes, block it by placing on any position other than corner;
		  else, use the position normally.
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 3
#define EMPTY ' '
#define P1 'X'
#define P2 'O'

typedef struct Result {
	int ok;
	char err_msg[50];
} Result;

typedef struct Position {
	int row;
	int col;
} Position;

typedef struct FindResult {
	int count;
	Position first_position;
} FindResult;

typedef struct MaybePosition {
	int some;
	Position value;
} MaybePosition;

void fill(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			board[i][j] = c;
}

FindResult find_in_row(char board[BOARD_SIZE][BOARD_SIZE], int row, char c) {
	Position first_pos;
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[row][i] == c) {
			if (count == 0) {
				first_pos.row = row;
				first_pos.col = i;
			}

			count++;
		}

	FindResult res = {.count = count, .first_position = first_pos};
	return res;
}

FindResult find_in_col(char board[BOARD_SIZE][BOARD_SIZE], int col, char c) {
	Position first_pos;
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[i][col] == c) {
			if (count == 0) {
				first_pos.row = i;
				first_pos.col = col;
			}

			count++;
		}

	FindResult res = {.count = count, .first_position = first_pos};
	return res;
}

FindResult find_top_left_diagonal(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	Position first_pos;
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++)
		if (board[i][i] == c) {
			if (count == 0) {
				first_pos.row = i;
				first_pos.col = i;
			}

			count++;
		}

	FindResult res = {.count = count, .first_position = first_pos};
	return res;
}

FindResult find_top_right_diagonal(char board[BOARD_SIZE][BOARD_SIZE], char c) {
	Position first_pos;
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		int j = (BOARD_SIZE - 1) - i;

		if (board[i][j] == c) {
			if (count == 0) {
				first_pos.row = i;
				first_pos.col = j;
			}

			count++;
		}
	}

	FindResult res = {.count = count, .first_position = first_pos};
	return res;
}

// Board

void draw(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			printf("%c", board[i][j]);

			if (j < BOARD_SIZE - 1)
				printf("|");
		}

		printf("\n");
	}
}

Result can_set(char board[BOARD_SIZE][BOARD_SIZE], Position pos) {
	if (pos.row >= BOARD_SIZE || pos.col >= BOARD_SIZE) {
		Result err = {.ok = false, .err_msg = "Espaco fora dos limites"};
		return err;
	}

	if (board[pos.row][pos.col] != EMPTY) {
		Result err = {.ok = false, .err_msg = "Espaco ocupado"};
		return err;
	}

	Result ok = {.ok = true};
	return ok;
}

Result set(char board[BOARD_SIZE][BOARD_SIZE], Position pos, char c) {
	Result res = can_set(board, pos);
	if (!res.ok)
		return res;

	board[pos.row][pos.col] = c;

	return res;
}

// Bot

Result finish_for_template(char board[BOARD_SIZE][BOARD_SIZE], FindResult target, FindResult empty) {
	if (target.count == BOARD_SIZE - 1 && empty.count == 1) {
		set(board, empty.first_position, P2);

		Result res = {.ok = true};
		return res;
	}

	Result res = {.ok = false, .err_msg = "Finishing position not found"};
	return res;
}

Result finish_for(char board[BOARD_SIZE][BOARD_SIZE], char for_c) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		// Horizontally.
		{
			FindResult target = find_in_row(board, i, for_c);
			FindResult empty = find_in_row(board, i, EMPTY);

			Result res = finish_for_template(board, target, empty);
			if (res.ok)
				return res;
		}
		
		// Vertically.
		{
			FindResult target = find_in_col(board, i, for_c);
			FindResult empty = find_in_col(board, i, EMPTY);

			Result res = finish_for_template(board, target, empty);
			if (res.ok)
				return res;
		}
	}

	// Diagonally (top left to bottom right).
	{
		FindResult target = find_top_left_diagonal(board, for_c);
		FindResult empty = find_top_left_diagonal(board, EMPTY);

		Result res = finish_for_template(board, target, empty);
		if (res.ok)
			return res;
	}

	// Diagonally (top right to bottom left).
	{
		FindResult target = find_top_right_diagonal(board, for_c);
		FindResult empty = find_top_right_diagonal(board, EMPTY);

		Result res = finish_for_template(board, target, empty);
		if (res.ok)
			return res;
	}

	Result err = {.ok = false, .err_msg = "No winning position found"};
	return err;
}

int pos_leads_to_enemy_triangle(char board[BOARD_SIZE][BOARD_SIZE], Position pos, char enemy) {
	/*
		Case:
		X (0, 2)
		A (0, 0)
		B (2, 2)

		A . X
		. . .
		. . B

		To find A and B, you need 4 positions, where 2 will always be equal to X (those are discarted);
		A = X where X.row = 0 OR X where x.row = 2 (pick the pos different from X)
		B = X where X.col = 0 OR X where x.col = 2 (same)
		
		Note: 2 = BOARD_SIZE - 1
	*/
}

MaybePosition corner(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < 4; i++) {
		int row = i <= 1 ? 0 : BOARD_SIZE - 1;
		int col = i % 2 == 0 ? 0 : BOARD_SIZE - 1;
		
		if (board[row][col] == EMPTY) {
			Position pos = {.row = row, .col = col};
			Result res = set(board, pos, P2);

			if (res.ok) {
				MaybePosition some_pos = {.some = true, .value = pos};
				return some_pos;
			}
		}
	}

	MaybePosition no_pos = {.some = false};
	return no_pos;
}

void play(char board[BOARD_SIZE][BOARD_SIZE]) {
	Result res;

	// 1. Win.
	res = finish_for(board, P2);
	if (res.ok)
		return;

	// 2. Block win.
	res = finish_for(board, P1);
	if (res.ok)
		return;

	// 3. Center.
	// The center is always (1, 1), no matter the board size. Different board sizes require
	// different strategies, so if this is the case, let the strategy be dumb.
	Position center = {.row = 1, .col = 1};
	res = set(board, center, P2);
	if (res.ok)
		return;

	MaybePosition corner_pos = corner(board);
	if (corner_pos.some) {
		// TEMP:
		int can_user_make_triangle = false;

		if (can_user_make_triangle) {
			// Place anywhere that is not a corner.
		} else {
			set(board, corner_pos.value, P2);
		}
	}

	/*
	1. Win
	2. Block win
	3. Center
	4. Prevent triangle <<<<<<<<<<<<<<<<<<<<<< #TODO
	X Y N
	. O .
	. . X
	O is the bot.
	Placing O on N will result in X successfuly making a triangle.
	Perhaps check if the corners next to N both have X; if yes, the corner N is a trap.
		Place O on any empty position that is not a corner, resulting in a draw.

	5. Corner
	6. Random (might never be needed, tests required)
	*/
}

// Game loop

int main() {
	char board[BOARD_SIZE][BOARD_SIZE];
	fill(board, EMPTY);

	while (true) {
		// User play

		int row, col;
		scanf("%i%i", &row, &col);

		Position pos = {.row = row, .col = col};
		Result res = set(board, pos, P1);
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
