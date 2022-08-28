#include <nds.h>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>
#include <string>

enum class Palette : u8 {
	white,
	gray,
	red,
	green,
	greenAlt,
	blue,
	yellow,
	blackWhite,
	blackRed,
	blackGreen,
	blackBlue,
};

static constexpr u16 palette[16][2] = {
	{0x0000, 0x7FFF}, // White
	{0x0000, 0x3DEF}, // Gray
	{0x0000, 0x001F}, // Red
	{0x0000, 0x03E0}, // Green
	{0x0000, 0x02E0}, // Green (alt)
	{0x0000, 0x656A}, // Blue
	{0x0000, 0x3339}, // Yellow
	{0x7FFF, 0x0000}, // Black on white
	{0x001F, 0x0000}, // Black on red
	{0x03E0, 0x0000}, // Black on green
	{0x656A, 0x0000}, // Black on blue
};

void Font();
void printf(int xPos, int yPos, Palette palette, const char *format, ...);
void printop(int xPos, int yPos, Palette palette, const char *format, ...);

