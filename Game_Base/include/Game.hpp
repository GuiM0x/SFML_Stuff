#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include "Fps.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics.hpp>

class Game
{
public:
	Game(const sf::VideoMode& videoMode, const std::string& title);

public:
	void run();

private:
	void processEvent();
	void inputHandler(const sf::Keyboard::Key& key, bool isPressed);
	void update(const sf::Time& dt);
	void render();

private:
	sf::RenderWindow m_window{};
	sf::Time m_timePerFrame{};
	Fps m_fps{};
	ResourceHolder<sf::Font, Fonts::ID> m_fontsHolder{};

	sf::RectangleShape m_player{};
	bool m_isMovingUp{false};
	bool m_isMovingLeft{false};
	bool m_isMovingDown{false};
	bool m_isMovingRight{false};
};

#endif // !GAME_HPP_INCLUDED
