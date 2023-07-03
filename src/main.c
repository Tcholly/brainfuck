#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <raylib.h>

#include "ui.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define CELL_NUMBER 30000
int8_t cells[CELL_NUMBER] = {0};
int current_cell = 0;

typedef struct
{
	int left, right;
} bracket_pair_t;

#define BRACKET_CACHE_MAX 64
bracket_pair_t bracket_cache[BRACKET_CACHE_MAX] = {0};
int bracket_pair_count = 0;

size_t instruction_index = 0;

// UI data
bool enable_ui = false;
int viewing_cell = -1;


void send_to_output(char ch)
{
	if (enable_ui)
	{
		// TODO:
	}
	else
		fputc(ch, stdout);
}


int find_matching_bracket(char* code, size_t len, int current_index)
{
	char current = code[current_index];
	for (int i = 0; i < bracket_pair_count; i++)
	{
		switch (current)
		{
			case '[':
				if (bracket_cache[i].left == current_index)
					return bracket_cache[i].right;
				break;
			case ']':
				if (bracket_cache[i].right == current_index)
					return bracket_cache[i].left;
				break;

			default:
				return -1;
		}
	}

	bracket_pair_t to_cache = {0};

	int increment = 0;
	char from = 0, to = 0;
	switch (current)
	{
		case '[':
			increment = 1;
			from = '[';
			to = ']';
			to_cache.left = current_index;
			break;
		case ']':
			increment = -1;
			from = ']';
			to = '[';
			to_cache.right = current_index;
			break;

		default:
			return -1;
	}

	int nesting = 0;
	current_index += increment;
	while (current_index >= 0 && current_index < (int)len)
	{
		if (code[current_index] == from)
			nesting++;
		else if (code[current_index] == to)
		{
			if (nesting == 0)
			{

				switch (current)
				{
					case '[':
						to_cache.right = current_index;
						break;
					case ']':
						to_cache.left = current_index;
						break;

					default:
						return -1;
				}
				
				if (bracket_pair_count < BRACKET_CACHE_MAX)
				{
					bracket_cache[bracket_pair_count] = to_cache;
					bracket_pair_count++;
				}

				return current_index;
			}
			nesting--;
		}
		current_index += increment;
	}

	return -1;
}

void step(char* code, size_t len)
{
	switch (code[instruction_index])
	{
		case '>':
			current_cell++;
			if (current_cell >= CELL_NUMBER)
			{
				printf("ERROR: Out of range\n");
				exit(222);
			}
			break;
		case '<':
			current_cell--;
			if (current_cell < 0)
			{
				printf("ERROR: Out of range\n");
				exit(111);
			}
			break;
		case '+':
			cells[current_cell] += 1;
			break;
		case '-':
			cells[current_cell] -= 1;
			break;
		case '.':
			send_to_output(cells[current_cell]);
			break;
		case ',':
			scanf("%c", &cells[current_cell]);
			break;
		case '[':
			if (cells[current_cell] == 0)
			{
				int matching_index = find_matching_bracket(code, len, instruction_index);
				if (matching_index < 0)
				{
					printf("ERROR: Couldn't find matching []");
					exit(2);
				}
				instruction_index = matching_index;
			}
			break;
		case ']':
			if (cells[current_cell] != 0)
			{
				int matching_index = find_matching_bracket(code, len, instruction_index);
				if (matching_index < 0)
				{
					printf("ERROR: Couldn't find matching []");
					exit(2);
				}
				instruction_index = matching_index;
			}
			break;

		default:
			break;
	}
}
void eval(char* code, size_t len)
{
	if (!code)
		return;

	while (instruction_index < len)
	{
		step(code, len);
		instruction_index++;
	}
}

size_t remove_garbage(char* code, size_t len)
{
	int garbage_start = -1;
	for (size_t i = 0; i < len; i++)
	{
		switch (code[i])
		{
			case '>':
			case '<':
			case '+':
			case '-':
			case '[':
			case ']':
			case '.':
			case ',':
				if (garbage_start >= 0)
				{
					memmove(code + garbage_start, code + i, len - i);
					int garbage_len = i - garbage_start;
					len -= garbage_len;
					i -= garbage_len;
					garbage_start = -1;
				}
				break;

			default:
				if (garbage_start < 0)
					garbage_start = i;
		}
	}

	return len;
}

void reset_state(void)
{
	instruction_index = 0;
	current_cell = 0;

	for (int i = 0; i < CELL_NUMBER; i++)
		cells[i] = 0;
}

void print_non_zero_cells()
{
	for (int i = 0; i < CELL_NUMBER; i++)
	{
		if (cells[i] != 0)
		{
			printf("[%d] = %d\n", i, cells[i]);
		}
	}
}

const char* eat_args(int* argc, char*** argv)
{
	if (*argc < 1)
		return NULL;

	*argc -= 1;
	return *(*argv)++;
}

void usage(const char* program)
{
	printf("USAGE: %s <file> [options]\n", program);
	printf("OPTIONS:\n");
	printf("\t--ui\tEnable ui\n");
}

int main(int argc, char** argv)
{
	const char* program = eat_args(&argc, &argv);

	if (argc < 1)
	{
		usage(program);
		return 69;
	}

	const char* filename = eat_args(&argc, &argv);

	if (argc > 0)
	{
		const char* option = eat_args(&argc, &argv);
		printf("%s\n", option);
		if (strcmp(option, "--ui") == 0)
			enable_ui = true;
	}

	FILE* file = fopen(filename, "r"); 
	if (!file)
	{
		printf("ERROR: Could not open file %s: %s\n", filename, strerror(errno));
		usage(program);
		return 42;
	}

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	rewind(file);

	char* content_buff = malloc(file_size + 1);
	if (!content_buff)
	{
		printf("ERROR: Failed to allocate memory");
		return 22;
	}
	
	fread(content_buff, 1, file_size, file);
	fclose(file);

	file_size = remove_garbage(content_buff, file_size);
	content_buff[file_size] = '\0';


	if (!enable_ui)
	{
		eval(content_buff, file_size);
		printf("\n----------------\n");
		print_non_zero_cells();
	}
	else
	{
		InitUI();

		bool run_code = false;
		bool code_done = false;

		while (!WindowShouldClose())
		{

			if ((IsKeyPressed(KEY_SPACE) || run_code) && !code_done)
			{
				step(content_buff, file_size);
				instruction_index++;

				if (instruction_index >= file_size)
					code_done = true;
			}

			if (IsKeyPressed(KEY_R))
			{
				run_code = !run_code;

				if (code_done)
				{
					run_code = true;
					code_done = false;
					reset_state();
				}

			}

			if (IsKeyPressed(KEY_LEFT))
			{
				if (viewing_cell < 0)
					viewing_cell = current_cell;

				viewing_cell--;

				viewing_cell = max(min(viewing_cell, CELL_NUMBER - 1), 0); 
			}

			if (IsKeyPressed(KEY_RIGHT))
			{
				if (viewing_cell < 0)
					viewing_cell = current_cell;

				viewing_cell++;

				viewing_cell = max(min(viewing_cell, CELL_NUMBER - 1), 0); 
			}

			if (IsKeyPressed(KEY_UP))
				viewing_cell = -1;

			BeginDrawing();
			ClearBackground(WHITE);
			
			DrawRectangleLines(0, 0, GetScreenWidth(), GetScreenHeight(), RED);
			DrawCode(content_buff, file_size, instruction_index);
			DrawCells(cells, current_cell, viewing_cell, CELL_NUMBER, GetScreenHeight() / 2, 100);

			EndDrawing();
		}
	}
	// printf("%.*s", (int)file_size, content_buff);

	free(content_buff);

	return 0;
}
