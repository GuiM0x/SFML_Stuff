#include "../include/Game.hpp"

Game::Game(const sf::VideoMode& videoMode, const std::string& title) :
	m_window{videoMode, title},
	m_player{sf::Vector2f{25.f, 25.f}}
{
	m_timePerFrame = sf::seconds(1.f / 60.f);
	m_fontsHolder.load(Fonts::ID::Digital, "resources/fonts/digital.ttf");
	m_fps.setFont(m_fontsHolder.get(Fonts::ID::Digital));
}

void Game::run()
{
	sf::Clock clock{};
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while(m_window.isOpen()){
		processEvent();
		timeSinceLastUpdate += clock.restart();
		while(timeSinceLastUpdate > m_timePerFrame){
			timeSinceLastUpdate -= m_timePerFrame;
			update(m_timePerFrame);
		}
		render();
	}
}

void Game::processEvent()
{
	sf::Event event{};
	while(m_window.pollEvent(event)){
		switch(event.type){
		case sf::Event::KeyPressed:
			inputHandler(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			inputHandler(event.key.code, false);
			break;
		case sf::Event::Closed:
			m_window.close();
			break;
		default:
			break;
		}
	}
}

void Game::inputHandler(const sf::Keyboard::Key& key, bool isPressed)
{
	if(key == sf::Keyboard::Z)
		m_isMovingUp = isPressed;
	else if(key == sf::Keyboard::Q)
		m_isMovingLeft = isPressed;
	else if(key == sf::Keyboard::S)
		m_isMovingDown = isPressed;
	else if(key == sf::Keyboard::D)
		m_isMovingRight = isPressed;

	if(key == sf::Keyboard::Escape)
		m_window.close();
}

void Game::update(const sf::Time& dt)
{
	const float distance{dt.asSeconds()*500.f};
	sf::Vector2f movement{};

	if(m_isMovingUp)
		movement.y -= distance;
	if(m_isMovingLeft)
		movement.x -= distance;
	if(m_isMovingDown)
		movement.y += distance;
	if(m_isMovingRight)
		movement.x += distance;

	m_player.move(movement);

	m_fps.update(dt);
}

void Game::render()
{
	m_window.clear();
	m_window.draw(m_player);
	m_window.draw(m_fps);
	m_window.display();
}
