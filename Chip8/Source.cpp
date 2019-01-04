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

void updateKeyboard(chip8 emuChip8);

int main(int argc, char **argv) {

	// check to see if the rom loaded properly
	if (!emuChip8.loadGame("pong.ch8"))
		return 1;

	// create a render window and set it's background color to black
	sf::RenderWindow window(sf::VideoMode(modified_width, modified_height), "Chip8");
	window.setFramerateLimit(60);
	window.clear();

	// emulation loop loop
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type = sf::Event::Closed)
				window.close();

			// exit if escape is pressed
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		updateKeyboard(emuChip8);
		emuChip8.emulateCycle();

		if (emuChip8.draw_flag == true) {
			window.clear();
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
}

void updateKeyboard(chip8 emuChip8) {
	emuChip8.key[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1);
	emuChip8.key[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2);
	emuChip8.key[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
	emuChip8.key[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4);

	emuChip8.key[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
	emuChip8.key[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
	emuChip8.key[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
	emuChip8.key[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);

	emuChip8.key[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
	emuChip8.key[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
	emuChip8.key[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
	emuChip8.key[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F);

	emuChip8.key[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z);
	emuChip8.key[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
	emuChip8.key[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
	emuChip8.key[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V);

}