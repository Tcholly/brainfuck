#pragma once

#include <stddef.h>
#include <stdint.h>

void InitUI(void);

void DrawCode(const char* code, size_t len, int instruction_index);
void DrawCells(const int8_t* cells, int current_cell, int view_cell, int max_cells, int y_pos, int size);
