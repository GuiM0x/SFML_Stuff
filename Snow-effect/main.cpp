#include <vector>
#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>

namespace gx{

int rollTheDice(int valmin, int valmax);
int rollTheDice(int valmin, int valmax)
{
    static std::mt19937 generator{static_cast<unsigned>(time(nullptr))};
    std::uniform_int_distribution<> dist{valmin, valmax};

    return dist(generator);
}

}

/////////////////////////////////////////////////////////////////////////////////////
//// SNOW EFFECT
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //// WINDOW
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Snow Effect", sf::Style::Default);

    //// RESSOURCES
    const std::size_t max_points{1000};
    sf::VertexArray particle{sf::Points, max_points};
    for(unsigned int i=0; i<max_points; ++i){
        float x{static_cast<float>(gx::rollTheDice(0, 1280))};
        float y{static_cast<float>(gx::rollTheDice(0, 720))};
        particle[i].position = sf::Vector2f(x, y);
        particle[i].color = sf::Color::Cyan;
    }

    std::vector<sf::Vector2f> all_speeds{};
    for(unsigned int i=0; i<max_points; ++i){
        all_speeds.push_back(sf::Vector2f(
            static_cast<float>(gx::rollTheDice(50, 100)),
            static_cast<float>(gx::rollTheDice(50, 200))
        ));
    }

    sf::Event event{};
    sf::Clock clock;
    sf::Time dt;

    //// GAMELOOP
    while (window.isOpen())
    {
        while (window.pollEvent(event)){
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // UPDATE
        dt = clock.restart();
        for(unsigned int i=0; i<max_points; ++i){
            particle[i].position.x += all_speeds[i].x * dt.asSeconds();
            particle[i].position.y += all_speeds[i].y * dt.asSeconds();
            if(particle[i].position.x > 1280.f){
                particle[i].position.x = 0.f;
                all_speeds[i].x = static_cast<float>(gx::rollTheDice(50, 100));
            }
            if(particle[i].position.y > 720.f){
                particle[i].position.y = 0.f;
                all_speeds[i].y = static_cast<float>(gx::rollTheDice(50, 200));
            }
        }

        // DRAW
        window.clear();
        window.draw(particle);
        window.display();
    }

    return 0;
}
