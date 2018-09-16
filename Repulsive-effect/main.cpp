#include <iostream>
#include <ctime>
#include <random>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

enum Key{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE,
    KEY_MAX
};

const float PI = 3.141592;

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

// DEG TO RAD
float radToDeg(float rad);
float radToDeg(float rad)
{
    return ((rad * 180.f) / PI);
}

// MOUSE POSITION TO "Vector2f"
sf::Vector2f getMousePosition(const sf::RenderWindow& window);
sf::Vector2f getMousePosition(const sf::RenderWindow& window)
{
    return sf::Vector2f{static_cast<float>(sf::Mouse::getPosition(window).x),
                        static_cast<float>(sf::Mouse::getPosition(window).y)};
}

double distanceFromMiddle(const sf::Vertex& vertex, const sf::Vector2f& mouse_pos);
double distanceFromMiddle(const sf::Vertex& vertex, const sf::Vector2f& mouse_pos)
{
    // Get the distance between a point and the center (mouse position)
    // By using Pythagore : sqrt((xp - xc)² + (yp - yx)²)
    return (sqrt(pow((vertex.position.x - mouse_pos.x), 2.0) + pow((vertex.position.y - mouse_pos.y), 2.0)));
}

// ADJUST PARTICLE ANGLE FROM CENTER TO 360 ACCORDING TO POSITION
float adjustRotationAngle(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float angle_degree);
float adjustRotationAngle(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float angle_degree)
{
    const float particle_x{particle.position.x};
    const float particle_y{particle.position.y};
    const float center_x{mouse_pos.x};
    const float center_y{mouse_pos.y};
    float new_angle{0.f};
    // Top-Left ?
    if(particle_x <= center_x && particle_y <= center_y){
        new_angle = 180.f + angle_degree;
    }
    // Top-Right ?
    if(particle_x >= center_x && particle_y <= center_y){
        new_angle = 359.f - angle_degree;
    }
    // Bottom-Left ?
    if(particle_x <= center_x && particle_y >= center_y){
        new_angle = 180.f - angle_degree;
    }
    // Bottom_Right ?
    if(particle_x >= center_x && particle_y >= center_y){
        new_angle = angle_degree;
    }

    // TO DO : Gérer le cas quand position ==

    return new_angle;
}

// GET PARTICLE ANGLE FROM CENTER (RETURN IN DEGREE)
float getParticleAngleFromCenter(const sf::Vertex& particle, const sf::Vector2f& mouse_pos);
float getParticleAngleFromCenter(const sf::Vertex& particle, const sf::Vector2f& mouse_pos)
{
    const float y_distance{static_cast<float>(sqrt(pow((mouse_pos.y - particle.position.y), 2.0)))};
    const float dist_from_mid{static_cast<float>(distanceFromMiddle(particle, mouse_pos))};
    const float sin_alpha{y_distance / dist_from_mid};
    const float angle{radToDeg(asin(sin_alpha))};
    return static_cast<float>(adjustRotationAngle(particle, mouse_pos, angle));
}

// CHECK IF PARTICLE IN BOUND
bool pointInBound(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float radius);
bool pointInBound(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float radius)
{
    const float x_left{mouse_pos.x - radius};
    const float x_right{mouse_pos.x + radius};
    const float y_top{mouse_pos.y - radius};
    const float y_bottom{mouse_pos.y + radius};
    return (particle.position.x >= x_left && particle.position.x <= x_right
            && particle.position.y >= y_top && particle.position.y <= y_bottom);
}

// CHECK IF PARTICLE IN CIRCLE
bool pointInCircle(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float radius);
bool pointInCircle(const sf::Vertex& particle, const sf::Vector2f& mouse_pos, float radius)
{
    return (distanceFromMiddle(particle, mouse_pos) <= radius);
}

// MOVE PARTICLE
void moveParticle(sf::Vertex& particle, const sf::Vector2f& mous_pos, float speed_dt);
void moveParticle(sf::Vertex& particle, const sf::Vector2f& mous_pos, float speed_dt)
{
    float angle{getParticleAngleFromCenter(particle, mous_pos)};
    float rad{degToRad(angle)};
    particle.position += sf::Vector2f{static_cast<float>(speed_dt * cos(rad)),
                                      static_cast<float>(speed_dt * sin(rad))};
}
/////////////////////////////////////////////////////////////////////////////////////
//// MAIN
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //// WINDOW
    const unsigned WIDTH_SCREEN{1280};
    const unsigned HEIGHT_SCREEN{720};
    sf::RenderWindow window{sf::VideoMode(WIDTH_SCREEN, HEIGHT_SCREEN), "Repulsive Effect", sf::Style::Default};

    //// RESSOURCES
    const unsigned max_particle{10000};
    sf::Vertex vertices[max_particle];
    sf::VertexBuffer particles{sf::Points};
    particles.create(max_particle);
    std::vector<float> particles_speed(max_particle);
    for(unsigned i=0; i<max_particle; ++i){
        vertices[i].position = sf::Vector2f{static_cast<float>(rollTheDice(0, WIDTH_SCREEN)),
                                            static_cast<float>(rollTheDice(0, HEIGHT_SCREEN))};
        vertices[i].color = sf::Color::Blue;
        particles_speed[i] = static_cast<float>(rollTheDice(30, 90));
    }
    const float mouse_radius{100.f};
    std::vector<bool> keys(Key::KEY_MAX, false);
    sf::Event event{};
    sf::Clock clock;
    sf::Time dt;

    //// GAMELOOP
    while (window.isOpen())
    {
        while (window.pollEvent(event)){
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Escape) window.close();
            }
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        //// UPDATE
        dt = clock.restart();
        particles.update(vertices);
        for(unsigned i=0; i<max_particle; ++i){
            if(pointInBound(vertices[i], getMousePosition(window), mouse_radius)){
                if(pointInCircle(vertices[i], getMousePosition(window), mouse_radius)){
                    if(vertices[i].color == sf::Color::Red){
                        vertices[i].color = sf::Color::Green;
                    } else {
                        vertices[i].color = sf::Color::Red;
                    }
                    moveParticle(vertices[i], getMousePosition(window), particles_speed[i] * dt.asSeconds());
                }
            }
        }

        //// DRAW
        window.clear();
        window.draw(particles);
        window.display();
    }

    return 0;
}
