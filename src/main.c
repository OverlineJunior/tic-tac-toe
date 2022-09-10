/*
NOTE: Bot strategy is hopefully done!
TODO: Victory detection.
TODO: Messages (victory, choose a position, etc).
TODO: (Optional) Use 0..BOARD_SIZE*2 position input instead of row and col.
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 3
#define EMPTY ' '
#define P1 'X'
#define P2 'O'

typedef struct{
	int ok;
	char err_msg[50];
} Result;

typedef struct {
	int row;
	int col;
} Position;

typedef struct {
	int count;
	Position first_position;
} FindResult;

typedef struct {
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
		if (board[row][i] == c)
		{
			if (count == 0)
			{
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
		if (board[i][col] == c)
		{
			if (count == 0)
			{
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
		if (board[i][i] == c)
		{
			if (count == 0)
			{
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

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		int j = (BOARD_SIZE - 1) - i;

		if (board[i][j] == c)
		{
			if (count == 0)
			{
				first_pos.row = i;
				first_pos.col = j;
			}

			count++;
		}
	}

	FindResult res = {.count = count, .first_position = first_pos};
	return res;
}

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

MaybePosition corner(char board[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < 4; i++)
	{
		int row = i <= 1 ? 0 : BOARD_SIZE - 1;
		int col = i % 2 == 0 ? 0 : BOARD_SIZE - 1;

		if (board[row][col] == EMPTY)
		{
			Position pos = {.row = row, .col = col};
			Result res = can_set(board, pos);

			if (res.ok)
			{
				MaybePosition some_pos = {.some = true, .value = pos};
				return some_pos;
			}
		}
	}

	MaybePosition no_pos;
	return no_pos;
}

int is_corner(char board[BOARD_SIZE][BOARD_SIZE], Position pos)
{
	int last = BOARD_SIZE - 1;
	return (pos.row == 0 || pos.row == last) && (pos.col == 0 || pos.col == last);
}

MaybePosition any_pos_except_corner(char board[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			Position pos = {.row = i, .col = j};
			Result res = can_set(board, pos);

			if (res.ok && !is_corner(board, pos))
			{
				MaybePosition m_pos = {.some = true, .value = pos};
				return m_pos;
			}
		}

	MaybePosition no_pos;
	return no_pos;
}

int corner_leads_to_enemy_triangle(char board[BOARD_SIZE][BOARD_SIZE], Position corner_pos, char enemy) {
	int last = BOARD_SIZE - 1;
	// Corners next to corner_pos.
	Position c1 = {.row = corner_pos.row == 0 ? last : 0, .col = corner_pos.col};
	Position c2 = {.row = corner_pos.row, .col = corner_pos.col == 0 ? last : 0};

	return board[c1.row][c1.col] == enemy && board[c2.row][c2.col] == enemy;
}

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

int has_winner(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < 2; i++) {
		char c = i == 0 ? P1 : P2;

		for (int j = 0; j < BOARD_SIZE; j++) {
			// Horizontally.
			{
				FindResult res = find_in_row(board, j, c);
				if (res.count == BOARD_SIZE)
					return true;
			}
			
			// Vertically.
			{
				FindResult res = find_in_col(board, j, c);
				if (res.count == BOARD_SIZE)
					return true;
			}
		}

		// Diagonally (top left to bottom right).
		{
			FindResult res = find_top_left_diagonal(board, c);
			if (res.count == BOARD_SIZE)
				return true;
		}

		// Diagonally (top right to bottom left).
		{
			FindResult res = find_top_right_diagonal(board, c);
			if (res.count == BOARD_SIZE)
				return true;
		}
	}

	return false;
}

void play(char board[BOARD_SIZE][BOARD_SIZE]) {
	/*
	Strategy:

	1. Win
	2. Block win
	3. Center
	4. Prevent triangle
		X Y N
		. O .
		. . X
		O is the bot.
		Placing O on N will result in X successfuly making a triangle.
		Check if the corners next to N both have X; if yes, the corner N is a trap.
		Place O on any empty position that is not a corner, forcing a draw.

	5. Corner
	6. Random (might never be needed, tests required)
	*/

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
	// different strategies, so if this is the case, let the strategy be dumb by design choice.
	Position center = {.row = 1, .col = 1};
	res = set(board, center, P2);
	if (res.ok)
		return;

	MaybePosition corner_pos = corner(board);
	if (corner_pos.some) {
		if (corner_leads_to_enemy_triangle(board, corner_pos.value, P1)) {
			MaybePosition pos = any_pos_except_corner(board);
			if (pos.some)
				set(board, pos.value, P2);
		} else {
			set(board, corner_pos.value, P2);
		}

		return;
	}

	// Catches every case.
	MaybePosition any_other = any_pos_except_corner(board);
	if (any_other.some)
		set(board, any_other.value, P2);
}

int main() {
	printf("\nCOMO JOGAR\n"
		   "Selecione uma posicao dando duas entradas de int, uma para a fileira e outra para a coluna.\n\n"
		   "NOTA\n"
		   "Esse programa foi feito pensando em suporte para diferentes tamanhos de tabuleiro."
		   " Para mudar o tamanho, modifique o valor da constante BOARD_SIZE definida no topo do codigo"
		   " (note que a estrategia do bot so e eficaz em um tabuleiro 3x3).\n\n");

	char board[BOARD_SIZE][BOARD_SIZE];
	fill(board, EMPTY);

	while (true) {
		// User
		int row, col;
		scanf("%i%i", &row, &col);

		Position pos = {.row = row, .col = col};
		Result res = set(board, pos, P1);
		if (!res.ok) {
			puts(res.err_msg);
			continue;
		}

		draw(board);

		if (has_winner(board)) {
			printf("Voce ganhou!");
			break;
		}

		// Bot
		printf("Bot:\n");

		play(board);
		draw(board);

		if (has_winner(board)) {
			printf("Voce perdeu.");
			break;
		}
	}
}
