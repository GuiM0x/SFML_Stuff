#include <iostream>
#include <SFML/Graphics.hpp>

/////////////////////////////////////////////////////////////////////////////////////
//// NO NAME
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //// WINDOW
    const unsigned WIDTH_SCREEN{1280};
    const unsigned HEIGHT_SCREEN{720};
    sf::RenderWindow window{sf::VideoMode(WIDTH_SCREEN, HEIGHT_SCREEN), "No Name", sf::Style::Default};

    //// RESSOURCES
    sf::Event event{};
    sf::Clock clock;
    sf::Time dt;

    //// GAMELOOP
    while (window.isOpen())
    {
        while (window.pollEvent(event)){
            if(event.type == sf::Event::KeyPressed){

            }
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Escape) window.close();
            }
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        //// UPDATE
        dt = clock.restart();

        //// DRAW
        window.clear();
        window.display();
    }

    return 0;
}
