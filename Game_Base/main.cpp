#include "include/Game.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
	Game game{sf::VideoMode{800, 600}, "Empty Game"};
	game.run();

	return 0;
}