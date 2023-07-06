#include "ui.h"
#include <raylib.h>


void InitUI()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 480, "Brainfuck interpreter");
	SetTargetFPS(60);
}

void DrawTextSpecial(const char* text, size_t len, int instruction_index, Vector2 pos)
{
	float pos_x = pos.x;
	// Vector2 pos = { 10.0f, 10.0f };
	for (int i = 0; i < (int)len; i++)
	{
		Color text_color = BLACK;
		if (i == instruction_index)
			text_color = GREEN;

		if (text[i] == '\n')
		{
			pos.x = pos_x;
			pos.y += 20.0f * 1.5f;
			continue;
		}
		// Vector2 text_size = MeasureChar(code[i], 20);
		DrawTextCodepoint(GetFontDefault(), text[i], pos, 20, text_color);
		pos.x += 14.0f;

		if (pos.x > GetScreenWidth() - 10.0f)
		{
			pos.x = pos_x;
			pos.y += 20.0f * 1.5f;
		}

	}
}

void DrawCells(const int8_t* cells, int current_cell, int view_cell, int max_cells, int y_pos, int size)
{
	if (view_cell < 0)
		view_cell = current_cell;

	int screen_width = GetScreenWidth();
	int visible_cells = screen_width / size + 2;

	// DrawLine(0, y_pos, screen_width, y_pos, RED);

	for (int i = -visible_cells / 2; i <= visible_cells / 2; i++)
	{
		int cell_index = view_cell + i;

		if (cell_index < 0 || cell_index >= max_cells)
			continue;

		Rectangle cell_rect = { .width = size, .height = size };
		cell_rect.x = (screen_width / 2.0f - size / 2.0f) + i * size;
		cell_rect.y = y_pos - size / 2.0f;

		DrawRectangleLinesEx(cell_rect, 2.0f, BLACK);

		const char* cell_content = TextFormat("%d", cells[cell_index]);
		int text_size = MeasureText(cell_content, 20);
		DrawText(cell_content, cell_rect.x + size / 2.0f - text_size / 2.0f, y_pos - 10, 20, BLACK);

		Color text_color = BLACK;
		if (cell_index == current_cell)
			text_color = GREEN;

		const char* cell_index_text = TextFormat("%d", cell_index);
		text_size = MeasureText(cell_index_text, 20);
		DrawText(cell_index_text, cell_rect.x + size / 2.0f - text_size / 2.0f, y_pos + size / 2 + 20 , 20, text_color);
	}
}
