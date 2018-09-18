#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>

const float PI{3.14159265};

/////////////////////////////////// RAY
class Ray : public sf::Drawable
{
public:
    // CTOR
    Ray(const sf::Vector2f& edgePoint) :
        m_edge_point{edgePoint} {}

    // FUNCTIONS
    void updateOrigin(const sf::Vector2f& new_origin){
        m_vertices[0].position = new_origin;
        updateAngleFromEdge();
    }
    void updateEndPoint(float factor){
        m_vertices[1].position = sf::Vector2f{m_vertices[0].position.x + static_cast<float>(cos(m_angle))*factor,
                                              m_vertices[0].position.y + static_cast<float>(sin(m_angle))*factor};
    }
    sf::Vector2f origin() const{
        return m_vertices[0].position;
    }
    sf::Vector2f endpoint() const{
        return m_vertices[1].position;
    }
    void setOffsetAngle(float offset){
        m_offset_angle = offset;
    }
    float angle() const{
        return m_angle;
    }

private:
    void updateAngleFromEdge(){
        const float dx = m_edge_point.x - m_vertices[0].position.x;
        const float dy = m_edge_point.y - m_vertices[0].position.y;
        m_angle = atan2(dy, dx) + m_offset_angle;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const{
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices{sf::LineStrip, 2};
    float m_angle{}; // radians
    const sf::Vector2f m_edge_point{};
    float m_offset_angle{0.f};
};

/////////////////////////////////// GET INTERSECT FACTOR
float getIntersectFactor(const Ray& ray, const sf::VertexArray& segment)
{
    const float r_px{ray.origin().x};
    const float r_py{ray.origin().y};
    const float r_dx = cos(ray.angle());
    const float r_dy = sin(ray.angle());

    const float s_px{segment[0].position.x};
    const float s_py{segment[0].position.y};
    const float s_dx{segment[1].position.x - segment[0].position.x};
    const float s_dy{segment[1].position.y - segment[0].position.y};

    float k1{}; // Factor to find
    float k2{}; // Factor determined and used to find k1

    k2 = ((r_dx*(s_py-r_py)) + (r_dy*(r_px-s_px)))/((s_dx*r_dy) - (s_dy*r_dx));
    k1 = (s_px+(s_dx*k2)-r_px)/r_dx;

    if(k1 >= 0 && k2 >= 0 && k2 <= 1)
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

/////////////////////////////////// GET EDGEPOINTS FROM BOX
void createRays(std::vector<Ray>& rays, const sf::RectangleShape& box)
{
    const sf::Vector2f position = box.getPosition();
    const sf::Vector2f size = box.getSize();
    const std::array<sf::Vector2f, 4> edgePoints{sf::Vector2f{position},
                                                 sf::Vector2f{position.x+size.x, position.y},
                                                 sf::Vector2f{position.x+size.x, position.y+size.y},
                                                 sf::Vector2f{position.x, position.y+size.y}};
    for(const auto& ep : edgePoints){
        Ray r{ep};
        rays.push_back(r);
        // Add two more rays (simple copy of original)
        // And apply negative and positive offset for their angle
        // It's needed to hit the wall(s) behind any given segment corner and it avoids some artifacts
        rays.push_back(r);
        rays.back().setOffsetAngle(-0.00001f);
        rays.push_back(r);
        rays.back().setOffsetAngle(+0.00001f);
    }
}

/////////////////////////////////// MOVE RAYS ORIGINS
void moveRaysOrigin(std::vector<Ray>& rays, const sf::Vector2f& new_origin)
{
    for(auto&& ray : rays){
        ray.updateOrigin(new_origin);
    }
}

/////////////////////////////////// MAIN
int main()
{
    sf::ContextSettings settings{};
    settings.antialiasingLevel = 8;

    // WINDOW
    const unsigned WINDOW_WIDTH{800};
    const unsigned WINDOW_HEIGHT{600};
    const sf::Vector2f SCREEN_CENTER{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f};
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Brouillon_SFML", sf::Style::Default, settings);

    // TIMER
    sf::Clock clock{};
    sf::Time dt{};

    // BOXES
    sf::RectangleShape box_window = createBox(0.f, 0.f, WINDOW_WIDTH, WINDOW_HEIGHT);
    sf::RectangleShape box_1 = createBox(50.f, 50.f, 150.f, 150.f);
    sf::RectangleShape box_2 = createBox(600.f, 50.f, 150.f, 150.f);
    sf::RectangleShape box_3 = createBox(600.f, 400.f, 150.f, 150.f);
    sf::RectangleShape box_4 = createBox(50.f, 400.f, 150.f, 150.f);
    sf::RectangleShape box_5 = createBox((WINDOW_WIDTH/2.f)-75.f, (WINDOW_HEIGHT/2.f)-75.f, 150.f, 150.f);

    // SEGMENTS
    std::vector<sf::VertexArray> stocked_segments{};
    getSegmentsFromBox(box_window, stocked_segments);
    getSegmentsFromBox(box_1, stocked_segments);
    getSegmentsFromBox(box_2, stocked_segments);
    getSegmentsFromBox(box_3, stocked_segments);
    getSegmentsFromBox(box_4, stocked_segments);
    getSegmentsFromBox(box_5, stocked_segments);

    // RAYS
    std::vector<Ray> rays{};
    createRays(rays, box_window);
    createRays(rays, box_1);
    createRays(rays, box_2);
    createRays(rays, box_3);
    createRays(rays, box_4);
    createRays(rays, box_5);

    // ENDPOINT (ONLY VISUAL)
    sf::CircleShape red_point{6.f};
    red_point.setFillColor(sf::Color::Green);
    std::vector<sf::CircleShape> visu_endpoint(rays.size(), red_point);

    // LOOP
    while(window.isOpen())
    {
        sf::Event event{};

        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Space){
                    std::cout << "Mouse_pos(" << sf::Mouse::getPosition(window).x << ", "
                                              << sf::Mouse::getPosition(window).y << ")" << '\n';
                }
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

        const sf::Vector2f mouse_pos{static_cast<float>(sf::Mouse::getPosition(window).x),
                                     static_cast<float>(sf::Mouse::getPosition(window).y)};
        moveRaysOrigin(rays, mouse_pos);

        unsigned ray_number{0}; // Only used for enpoint visu
        for(auto&& ray : rays){
            std::vector<float> factors{};
            for(const auto& seg : stocked_segments){
                float factor{getIntersectFactor(ray, seg)};
                if(factor >= 0.f)
                    factors.push_back(factor);
            }
            if(!factors.empty()){
                std::sort(factors.begin(), factors.end());
                ray.updateEndPoint(factors[0]);
            }
            visu_endpoint[ray_number].setPosition(ray.endpoint().x - red_point.getRadius(),
                                                  ray.endpoint().y - red_point.getRadius());
            ++ray_number;
        }

        // DRAW
        window.clear();
        for(const auto& ray : rays) window.draw(ray);
        for(const auto& red_p : visu_endpoint) window.draw(red_p);
        window.draw(box_window);
        window.draw(box_1);
        window.draw(box_2);
        window.draw(box_3);
        window.draw(box_4);
        window.draw(box_5);
        window.display();
    }

    return 0;
}
