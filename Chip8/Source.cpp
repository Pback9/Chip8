#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "chip8.h"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

int resize_modifier = 10;

int modified_height = DISPLAY_HEIGHT * resize_modifier;
int modified_width = DISPLAY_WIDTH * resize_modifier;

//chip8 emuChip8;

int main() {

	sf::Window window(sf::VideoMode(modified_width, modified_height), "Chip8");

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type = sf::Event::Closed)
				window.close();
		}
	}
}