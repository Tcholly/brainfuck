#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>


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


void eval(char* code, size_t len)
{
	if (!code)
		return;

	for (size_t i = 0; i < len; i++)
	{
		switch (code[i])
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
				fputc(cells[current_cell], stdout);
				break;
			case ',':
				scanf("%c", &cells[current_cell]);
				break;
			case '[':
				if (cells[current_cell] == 0)
				{
					int matching_index = find_matching_bracket(code, len, i);
					if (matching_index < 0)
					{
						printf("ERROR: Couldn't find matching []");
						exit(2);
					}
					i = matching_index;
				}
				break;
			case ']':
				if (cells[current_cell] != 0)
				{
					int matching_index = find_matching_bracket(code, len, i);
					if (matching_index < 0)
					{
						printf("ERROR: Couldn't find matching []");
						exit(2);
					}
					i = matching_index;
				}
				break;

			default:
				break;
		}
	}
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
	printf("USAGE: %s <file>\n", program);
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
	
	fread(content_buff, 1, file_size, file);
	content_buff[file_size] = '\0';
	fclose(file);

	eval(content_buff, file_size);
	printf("\n----------------\n");
	print_non_zero_cells();

	free(content_buff);

	return 0;
}
