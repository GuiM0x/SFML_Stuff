#include <iostream>
#include <SFML/Graphics.hpp>

static sf::Vector2f mouse_position(sf::RenderWindow&);
static sf::Vector2f mouse_position(sf::RenderWindow& window)
{
    return sf::Vector2f{static_cast<float>(sf::Mouse::getPosition(window).x),
                        static_cast<float>(sf::Mouse::getPosition(window).y)};
}
/////////////////////////////////////////////////////////////////////////////////////
//// MAIN
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    sf::RenderWindow window{sf::VideoMode(1280, 750), "Click-n-Drag", sf::Style::Close};
    const sf::Vector2f player_size{25.f, 25.f};
    const sf::Vector2f initial_player_pos{250.f, 500.f};
    sf::RectangleShape player{player_size};
    player.setPosition(initial_player_pos);
    sf::FloatRect player_box{player.getGlobalBounds()};
    sf::Vector2f offset_drag{};
    bool isDrag{false};

    //// GAMELOOP
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            // KeyBoard EventReleased
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
            // Mouse Event Pressed
            if(event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    if(player_box.contains(mouse_position(window))){
                        isDrag = true;
                        offset_drag = sf::Vector2f{mouse_position(window).x - player.getPosition().x,
                                                   mouse_position(window).y - player.getPosition().y};
                    }
                }
                if(event.mouseButton.button == sf::Mouse::Right){
                    player.setPosition(sf::Vector2f{mouse_position(window).x - (player.getGlobalBounds().width / 2.f),
                                                    mouse_position(window).y - (player.getGlobalBounds().height / 2.f)});
                }
            }
            //Mouse Event Released
            if(event.type == sf::Event::MouseButtonReleased){
                if(event.mouseButton.button == sf::Mouse::Left){
                    if(isDrag){
                        isDrag = false;
                    }
                }
            }
            // Window Events
            if(event.type == sf::Event::Closed)
                window.close();
        }

        // Update
        if(isDrag){
            player.setPosition(sf::Vector2f{mouse_position(window).x - offset_drag.x,
                                            mouse_position(window).y - offset_drag.y});
        }
        player_box = player.getGlobalBounds();

        // Draw
        window.clear();
        window.draw(player);
        window.display();
    }

    return 0;
}
