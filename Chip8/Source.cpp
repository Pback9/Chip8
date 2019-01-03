#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "chip8.h"

// display dimensions setup
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

int resize_modifier = 10;

int modified_height = DISPLAY_HEIGHT * resize_modifier;
int modified_width = DISPLAY_WIDTH * resize_modifier;

chip8 emuChip8;

int main(int argc, char **argv) {

	// check to see if the rom loaded properly
	if (!emuChip8.loadGame("pong.ch8"))
		return 1;

	// create a render window and set it's background color to black
	sf::RenderWindow window(sf::VideoMode(modified_width, modified_height), "Chip8");
	window.clear();

	// execution loop
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type = sf::Event::Closed)
				window.close();
		}

		// update drawing
		sf::RectangleShape pixel(sf::Vector2f(resize_modifier, resize_modifier));

		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				if (emuChip8.gfx[y * 64 + x] == 1) {
					pixel.setPosition(x * resize_modifier, y * resize_modifier);
					window.draw(pixel);
				}
			}
		}
		
		window.display();
	}
}