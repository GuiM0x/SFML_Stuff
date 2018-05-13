#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>

const float PI{3.141592};

enum Key{
    LEFT,
    RIGHT,
    SPACE,
    KEY_MAX
};

/////////////////////////////////////////////////////////////////////////////////////
//// FUNCTIONNAL
/////////////////////////////////////////////////////////////////////////////////////
// RANDOM GENERATOR
int rollTheDice(int valmin, int valmax);
int rollTheDice(int valmin, int valmax)
{
    static std::mt19937 generator{static_cast<unsigned>(time(nullptr))};
    std::uniform_int_distribution<> dist{valmin, valmax};

    return dist(generator);
}

// DEG TO RAD
float degToRad(float angle_degree);
float degToRad(float angle_degree)
{
    return ((angle_degree * PI) / 180.f);
}

// GET A POINT ON CIRCLE FROM ANGLE
// Function to get a point's position on circle
// Arg 1 : Angle (in degree), Arg 2 : Radius, Arg 3 : circle center position
sf::Vector2f getCirclePointFromAngle(float angle_degree, float radius, const sf::Vector2f& center);
sf::Vector2f getCirclePointFromAngle(float angle_degree, float radius, const sf::Vector2f& center)
{
    // The cosinus and sinus function take a radian angle
    // So we simply use the formula conversion deg->rad on our angle
    const float angle_rad{degToRad(angle_degree)};
    const float cosinus{static_cast<float>(cos(angle_rad))};
    const float sinus{static_cast<float>(sin(angle_rad))};
    // Then we can get x and y with the appropriate formula
    return sf::Vector2f{center.x + (radius * cosinus),
                        center.y + (radius * sinus)};
}

// MOVE PARTICLE
sf::Vector2f moveParticle(float speed, float angle_degree, const sf::Vector2f& current_pos);
sf::Vector2f moveParticle(float speed, float angle_degree, const sf::Vector2f& current_pos)
{
    const float angle_rad{degToRad(angle_degree)};
    const float cosinus{static_cast<float>(cos(angle_rad))};
    const float sinus{static_cast<float>(sin(angle_rad))};
    // Formula with speed : x0 + v0 * cos(A)
    //                      y0 + v0 * sin(A)
    // where x0 and y0 are the current position, v0 is speed * deltaTime and A the angle from origin
    // return the next position
    return sf::Vector2f{current_pos.x + (speed * cosinus),
                        current_pos.y + (speed * sinus)};
}

// ROTATE SPRAYER
void updatePole(sf::Vector2f& pole_position, float& pole_angle, float rotation, float radius, const sf::Vector2f& center);
void updatePole(sf::Vector2f& pole_position, float& pole_angle, float rotation, float radius, const sf::Vector2f& center)
{
    pole_angle += rotation;
    if(pole_angle >= 360) pole_angle = 0.f;
    pole_position = sf::Vector2f{getCirclePointFromAngle(pole_angle, radius, center)};
}

// CHECK FOR PARTICLE OUTSIDE SCREEN
bool checkOutScreenParticle(const sf::Vertex& point, unsigned screen_width, unsigned screen_height);
bool checkOutScreenParticle(const sf::Vertex& point, unsigned screen_width, unsigned screen_height)
{
    const float x = point.position.x;
    const float y = point.position.y;
    if(x <= 0.f || x >= screen_width || y <= 0.f || y >= screen_height)
        return true;
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////
//// MAIN
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //// WINDOW
    const unsigned WIDTH_SCREEN{1280};
    const unsigned HEIGHT_SCREEN{720};
    sf::RenderWindow window{sf::VideoMode(WIDTH_SCREEN, HEIGHT_SCREEN), "No Name", sf::Style::Default};
    //window.setFramerateLimit(60);

    //// RESSOURCES
    const float sprayer_radius{30.f};
    sf::CircleShape sprayer{sprayer_radius, 30};
    sprayer.setPosition((WIDTH_SCREEN / 2.f) - sprayer.getRadius(),
                        (HEIGHT_SCREEN / 2.f) - sprayer.getRadius());
    sf::Texture sprayer_texture{};
    sprayer_texture.loadFromFile("assets/sprayer.png");
    sf::Sprite sprayer_sprite(sprayer_texture);
    sprayer_sprite.setOrigin(sprayer_radius, sprayer_radius);
    sprayer_sprite.setPosition(sprayer.getPosition() + sf::Vector2f{sprayer_radius, sprayer_radius});
    const unsigned max_particle{1000};
    const sf::Vector2f sprayer_center{sprayer.getPosition().x + sprayer.getRadius(),
                                      sprayer.getPosition().y + sprayer.getRadius()};
    float north_angle{270.f};
    float south_angle{90.f};
    sf::Vector2f north_pole{getCirclePointFromAngle(north_angle, sprayer.getRadius(), sprayer_center)};
    sf::Vector2f south_pole{getCirclePointFromAngle(south_angle, sprayer.getRadius(), sprayer_center)};
    sf::VertexArray north_particle{sf::Points, max_particle};
    sf::VertexArray south_particle{sf::Points, max_particle};
    std::vector<float> north_speed(max_particle);
    std::vector<float> south_speed(max_particle);
    std::vector<float> north_particle_current_angle(max_particle);
    std::vector<float> south_particle_current_angle(max_particle);
    for(unsigned i = 0; i < max_particle; ++i){
        north_particle[i].color         = sf::Color::Red;
        south_particle[i].color         = sf::Color::Red;
        north_particle[i].position      = north_pole;
        south_particle[i].position      = south_pole;
        north_speed[i]                  = static_cast<float>(rollTheDice(50, 300));
        south_speed[i]                  = static_cast<float>(rollTheDice(50, 300));
        north_particle_current_angle[i] = north_angle;
        south_particle_current_angle[i] = south_angle;
    }
    sf::Texture aura{};
    aura.loadFromFile("assets/aura.png");
    std::vector<sf::Sprite> north_aura(max_particle, sf::Sprite{aura});
    std::vector<sf::Sprite> south_aura(max_particle, sf::Sprite{aura});
    std::vector<bool> keys(Key::KEY_MAX, false);
    sf::Event event{};
    sf::Clock clock;
    sf::Time dt;
    const float time_refresh{1.f};
    float elapsed_refresh{0.f};
    const float rotation_speed_sprayer{100.f};

    //// GAMELOOP
    while (window.isOpen())
    {
        while (window.pollEvent(event)){
            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Q)     keys[LEFT]  = true;
                if(event.key.code == sf::Keyboard::D)     keys[RIGHT] = true;
                if(event.key.code == sf::Keyboard::Space) keys[SPACE] = true;
            }
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Q)      keys[LEFT]  = false;
                if(event.key.code == sf::Keyboard::D)      keys[RIGHT] = false;
                if(event.key.code == sf::Keyboard::Space)  keys[SPACE] = false;
                if(event.key.code == sf::Keyboard::Escape) window.close();
            }
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        //// UPDATE
        dt = clock.restart();
        if(keys[SPACE]){
            // The roation to apply must be proportinnal to time,
            // so the base const speed must be multiply by deltaTime
            const float rotation{rotation_speed_sprayer * dt.asSeconds()};
            updatePole(north_pole, north_angle, rotation, sprayer_radius, sprayer_center);
            updatePole(south_pole, south_angle, rotation, sprayer_radius, sprayer_center);
            sprayer_sprite.setRotation(north_angle);

        }
        for(unsigned i = 0; i < max_particle; ++i){
            north_particle[i].position = moveParticle(north_speed[i] * dt.asSeconds(),
                                                      north_particle_current_angle[i],
                                                      north_particle[i].position);
            south_particle[i].position = moveParticle(south_speed[i] * dt.asSeconds(),
                                                      south_particle_current_angle[i],
                                                      south_particle[i].position);
            if(checkOutScreenParticle(north_particle[i], WIDTH_SCREEN, HEIGHT_SCREEN)){
                north_particle[i].position = north_pole;
                north_particle_current_angle[i] = north_angle;
            }
            if(checkOutScreenParticle(south_particle[i], WIDTH_SCREEN, HEIGHT_SCREEN)){
                south_particle[i].position = south_pole;
                south_particle_current_angle[i] = south_angle;
            }
            north_aura[i].setPosition(north_particle[i].position - sf::Vector2f{north_aura[i].getGlobalBounds().width/2.f,
                                                                                north_aura[i].getGlobalBounds().height/2.f});
            south_aura[i].setPosition(south_particle[i].position - sf::Vector2f{south_aura[i].getGlobalBounds().width/2.f,
                                                                                south_aura[i].getGlobalBounds().height/2.f});
        }
        elapsed_refresh += dt.asSeconds();
        if(elapsed_refresh >= time_refresh){
            elapsed_refresh = 0.f;
            std::cout << 1.f / dt.asSeconds() << '\n';
        }

        //// DRAW
        window.clear();
        window.draw(sprayer_sprite);
        window.draw(north_particle);
        window.draw(south_particle);
        for(const auto& aura : north_aura){
            window.draw(aura);
        }
        for(const auto& aura : south_aura){
            window.draw(aura);
        }
        window.display();
    }

    return 0;
}
