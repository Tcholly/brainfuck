#pragma once

#include "raylib.h"
#include <stddef.h>
#include <stdint.h>

void InitUI(void);

void DrawTextSpecial(const char* code, size_t len, int instruction_index, Vector2 pos);
void DrawCells(const int8_t* cells, int current_cell, int view_cell, int max_cells, int y_pos, int size);
