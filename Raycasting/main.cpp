#include <iostream>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

const float PI{3.141592};

/////////////////////////////////// RAD TO DEG
float radToDeg(float angle_deg)
{
    return float{(angle_deg * 180.f)/PI};
}

/////////////////////////////////// DEG TO RAD
float degToRad(float angle_rad)
{
    return float{(angle_rad * PI)/180.f};
}

/////////////////////////////////// CREATE ENDPOINT
sf::Vector2f setEndPoint(const sf::Vector2f& origin, float angle, float factor)
{
    return sf::Vector2f{origin.x + static_cast<float>(cos(degToRad(angle)))*factor,
                        origin.y + static_cast<float>(sin(degToRad(angle)))*factor};
}

/////////////////////////////////// GET INTERSECT FACTOR
float getIntersectFactor(const sf::VertexArray& ray, float ray_angle, const sf::VertexArray& segment)
{
    float k1{}; // Factor to find
    float k2{}; // Factor determined and used to find k1

    const float r_px{ray[0].position.x};
    const float r_py{ray[0].position.y};
    const float r_dx = cos(degToRad(ray_angle));
    const float r_dy = sin(degToRad(ray_angle));

    const float s_px{segment[0].position.x};
    const float s_py{segment[0].position.y};
    const float s_dx{segment[1].position.x - segment[0].position.x};
    const float s_dy{segment[1].position.y - segment[0].position.y};

    k2 = ((r_dx*(s_py-r_py)) + (r_dy*(r_px-s_px)))/((s_dx*r_dy) - (s_dy*r_dx));
    k1 = (s_px+(s_dx*k2)-r_px)/r_dx;

    if(k1 > 0 && k2 > 0 && k2 < 1)
        return k1;

    return float{-1.f};
}

/////////////////////////////////// CREATE BOX
sf::RectangleShape createBox(float x, float y, float width, float height)
{
    sf::RectangleShape r{sf::Vector2f{width, height}};
    r.setPosition(x, y);
    r.setOutlineColor(sf::Color::White);
    r.setOutlineThickness(1);
    r.setFillColor(sf::Color::Transparent);
    return r;
}

/////////////////////////////////// GET SEGMENTS FROM BOX
void getSegmentsFromBox(const sf::RectangleShape& box, std::vector<sf::VertexArray>& stocked_segments)
{
    const sf::Vector2f position = box.getPosition();
    const sf::Vector2f size = box.getSize();

    sf::VertexArray m_line_AB{sf::LineStrip, 2};
    m_line_AB[0].position = position;
    m_line_AB[1].position = sf::Vector2f{position.x + size.x, position.y};
    stocked_segments.push_back(m_line_AB);

    sf::VertexArray m_line_BC{sf::LineStrip, 2};
    m_line_BC[0].position = sf::Vector2f{position.x + size.x, position.y};
    m_line_BC[1].position = sf::Vector2f{position.x + size.x, position.y + size.y};
    stocked_segments.push_back(m_line_BC);

    sf::VertexArray m_line_DC{sf::LineStrip, 2};
    m_line_DC[0].position = sf::Vector2f{position.x, position.y + size.y};
    m_line_DC[1].position = sf::Vector2f{position.x + size.x, position.y + size.y};
    stocked_segments.push_back(m_line_DC);

    sf::VertexArray m_line_AD{sf::LineStrip, 2};
    m_line_AD[0].position = position;
    m_line_AD[1].position = sf::Vector2f{position.x, position.y + size.y};
    stocked_segments.push_back(m_line_AD);
}

/////////////////////////////////// MOVE RAYS ORIGINS
void moveRaysOrigins(std::vector<sf::VertexArray>& rays, const sf::Vector2f& new_origin)
{
    for(auto&& ray : rays){
        ray[0].position = new_origin;
    }
}

/////////////////////////////////// MAIN
int main()
{
    // WINDOW
    const unsigned WINDOW_WIDTH{800};
    const unsigned WINDOW_HEIGHT{600};
    const sf::Vector2f SCREEN_CENTER{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f};
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Brouillon_SFML");

    // TIMER
    sf::Clock clock{};
    sf::Time dt{};

    // A RAY
    const unsigned MAX_RAYS{32};
    std::vector<sf::VertexArray> rays{};
    for(unsigned i=0; i<MAX_RAYS; ++i){
        sf::VertexArray ray{sf::LineStrip, 2};
        ray[0].position = SCREEN_CENTER;
        ray[1].position = setEndPoint(SCREEN_CENTER, (359.f/MAX_RAYS)*i, 1.f);
        rays.push_back(ray);
    }

    // BOXES
    sf::RectangleShape box_1 = createBox(100.f, 100.f, 150.f, 350.f);
    sf::RectangleShape box_2 = createBox(500.f, 100.f, 150.f, 350.f);

    // SEGMENTS
    std::vector<sf::VertexArray> stocked_segments{};
    getSegmentsFromBox(box_1, stocked_segments);
    getSegmentsFromBox(box_2, stocked_segments);

    // LOOP
    while(window.isOpen())
    {
        sf::Event event{};

        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Escape){
                    window.close();
                }
            }

            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        // UPDATE
        dt = clock.restart();

        float angle{0.f};
        unsigned ray_number{0};
        for(auto&& ray : rays){
            std::vector<float> factors{};
            for(const auto& seg : stocked_segments){
                float factor{getIntersectFactor(ray, angle, seg)};
                if(factor > 0.f)
                    factors.push_back(factor);
            }
            std::sort(factors.begin(), factors.end());
            if(!factors.empty())
                ray[1].position = setEndPoint(ray[0].position, angle, factors[0]);
            else
                ray[1].position = setEndPoint(ray[0].position, angle, 1000.f);
            ++ray_number;
            angle = (359.f/MAX_RAYS)*ray_number;
        }

        const sf::Vector2f mouse_pos{static_cast<float>(sf::Mouse::getPosition(window).x),
                                     static_cast<float>(sf::Mouse::getPosition(window).y)};
        moveRaysOrigins(rays, mouse_pos);

        // DRAW
        window.clear();
        for(const auto& ray : rays){
            window.draw(ray);
        }
        window.draw(box_1);
        window.draw(box_2);
        window.display();
    }

    return 0;
}
