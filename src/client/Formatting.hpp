#ifndef ROGUELIKE_FORMATTING_HPP_INCLUDED
#define ROGUELIKE_FORMATTING_HPP_INCLUDED

enum class Color {
	 BLACK = 30,
	 RED,
	 GREEN,
	 YELLOW,
	 BLUE,
	 MAGENTA,
	 CYAN,
	 WHITE,
	 NO_COLOR
};

struct Formating {
	Color text_color;
	Color background_color;
	// TODO: add bold, curved etc...
};

#endif
