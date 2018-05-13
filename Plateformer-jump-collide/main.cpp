#include <vector>
#include <SFML/Graphics.hpp>

enum Key{
    LEFT,
    RIGHT,
    JUMP,
    KEY_MAX
};
/////////////////////////////////////////////////////////////////////////////////////
//// Jump ! Jump !
/////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //// WINDOW
    const unsigned WIDTH_SCREEN{1280};
    const unsigned HEIGHT_SCREEN{720};
    sf::RenderWindow window{sf::VideoMode(WIDTH_SCREEN, HEIGHT_SCREEN), "Jump ! Jump !", sf::Style::Default};

    //// RESSOURCES
    const sf::Vector2f player_size{50.f, 50.f};
    sf::RectangleShape player{player_size};
    player.setPosition(0.f, HEIGHT_SCREEN - player_size.y);
    const sf::Vector2f platform_size{150.f, 75.f};
    sf::RectangleShape platform{platform_size}, platform2{platform_size};
    platform.setPosition(300.f, HEIGHT_SCREEN - platform_size.y);
    platform.setFillColor(sf::Color(0, 150, 255));
    platform2.setPosition(750.f, HEIGHT_SCREEN - (platform_size.y)*2.f);
    platform2.setFillColor(sf::Color(0, 150, 255));
    std::vector<bool> keys(Key::KEY_MAX, false);
    const float ground_limit{HEIGHT_SCREEN};
    const float velocity_x{200.f};
    float velocity_y{0.f};
    bool is_jumping{false};
    bool on_ground{true};
    sf::Event event{};
    sf::Clock clock;
    sf::Time dt;

    //// GAMELOOP
    while (window.isOpen())
    {
        while (window.pollEvent(event)){
            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Q)     keys[LEFT]  = true;
                if(event.key.code == sf::Keyboard::D)     keys[RIGHT] = true;
                if(event.key.code == sf::Keyboard::Space) keys[JUMP]  = true;
            }
            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Q)      keys[LEFT]  = false;
                if(event.key.code == sf::Keyboard::D)      keys[RIGHT] = false;
                if(event.key.code == sf::Keyboard::Space)  keys[JUMP]  = false;
                if(event.key.code == sf::Keyboard::Escape) window.close();
            }
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        //// UPDATE
        dt = clock.restart();

        player.move(0.f, dt.asSeconds() * velocity_y * (-1));
        if(keys[LEFT])  player.move(dt.asSeconds() * velocity_x * (-1), 0.f);
        if(keys[RIGHT]) player.move(dt.asSeconds() * velocity_x * 1, 0.f);
        // If the player is not in the air OR not on the ground, so the gravity works
        // When the player is on a platform, he's not considered as on ground
        // So the gravity push him down but the platform cancel that sort of gravity by setting
        // the player's velocity_y a 0.
        if(is_jumping || !on_ground ){
            velocity_y -= (1200.f * dt.asSeconds());
        }
        // Launch the jump
        if(keys[JUMP] && !is_jumping){
            is_jumping = true;
            on_ground = false;
            velocity_y = 600.f;
        }
        if(player.getPosition().y + player_size.y > ground_limit){
            is_jumping = false;
            on_ground = true;
            velocity_y = 0.f;
            player.setPosition(player.getPosition().x, ground_limit - player_size.y);
        }

        const sf::FloatRect player_box = player.getGlobalBounds();
        sf::FloatRect platform_box = platform.getGlobalBounds();
        if(!player_box.intersects(platform_box)) { platform_box = platform2.getGlobalBounds(); }
        if(player_box.intersects(platform_box)){
            const float PLAYER_TOP{player_box.top};
            const float PLAYER_BOTTOM{player_box.top + player_box.height};
            const float PLAYER_LEFT{player_box.left};
            const float PLAYER_RIGHT{player_box.left + player_box.width};
            const float PLATFORM_TOP{platform_box.top};
            const float PLATFORM_BOTTOM{platform_box.top + platform_box.height};
            const float PLATFORM_LEFT{platform_box.left};
            const float PLATFORM_RIGHT{platform_box.left + platform_box.width};
            // LEFT SIDE ?
            if(PLAYER_LEFT <= PLATFORM_LEFT && PLAYER_RIGHT >= PLATFORM_LEFT){
                // Top Corner ?
                if(PLAYER_TOP <= PLATFORM_TOP && PLAYER_BOTTOM >= PLATFORM_TOP){
                    if(((PLAYER_RIGHT - PLATFORM_LEFT) > (PLAYER_BOTTOM - PLATFORM_TOP))){
                        // Test if player is falling (negative velocity).
                        // If it is, the player is stopped by set velocity to 0,
                        // Otherwise, the player continue to going up whith current velocity
                        if(velocity_y < 0.f){
                            is_jumping = false;
                            velocity_y = 0.f;
                            // replace on top
                            player.setPosition(PLAYER_LEFT, PLATFORM_TOP - player_box.height);
                        }
                    } else {
                        // replace on left
                        player.setPosition(PLATFORM_LEFT - player_box.width, PLAYER_TOP);
                    }
                }
                // Middle ?
                if(PLAYER_TOP >= PLATFORM_TOP && PLAYER_BOTTOM <= PLATFORM_BOTTOM){
                    // replace on left
                    player.setPosition(PLATFORM_LEFT - player_box.width, PLAYER_TOP);
                }
                // Bottom Corner ?
                if(PLAYER_TOP <= PLATFORM_BOTTOM && PLAYER_BOTTOM >= PLATFORM_BOTTOM){
                    if((PLAYER_RIGHT - PLATFORM_LEFT) > (PLATFORM_BOTTOM - PLAYER_TOP)){
                        // replace on bottom
                        player.setPosition(PLAYER_LEFT, PLATFORM_BOTTOM);
                        if(velocity_y > 0.f) velocity_y = 0.f;
                    } else {
                        // replace on left
                        player.setPosition(PLATFORM_LEFT - player_box.width, PLAYER_TOP);
                    }
                }
            }
            // RIGHT SIDE ?
            else if(PLAYER_LEFT <= PLATFORM_RIGHT && PLAYER_RIGHT >= PLATFORM_RIGHT){
                // Top Corner ?
                if(PLAYER_TOP <= PLATFORM_TOP && PLAYER_BOTTOM >= PLATFORM_TOP){
                    if((PLATFORM_RIGHT - PLAYER_LEFT) > (PLAYER_BOTTOM - PLATFORM_TOP)){
                        // Test if player is falling (negative velocity).
                        // If it is, the player is stopped by set velocity to 0,
                        // Otherwise, the player continue to going up whith current velocity
                        if(velocity_y < 0.f){
                            is_jumping = false;
                            velocity_y = 0.f;
                            // replace on top
                            player.setPosition(PLAYER_LEFT, PLATFORM_TOP - player_box.height);
                        }
                    } else {
                        // replace on right
                        player.setPosition(PLATFORM_RIGHT, PLAYER_TOP);
                    }
                }
                // Middle ?
                if(PLAYER_TOP >= PLATFORM_TOP && PLAYER_BOTTOM <= PLATFORM_BOTTOM){
                    // replace on right
                    player.setPosition(PLATFORM_RIGHT, PLAYER_TOP);
                }
                // Bottom Corner ?
                if(PLAYER_TOP <= PLATFORM_BOTTOM && PLAYER_BOTTOM >= PLATFORM_BOTTOM){
                    if((PLATFORM_RIGHT - PLAYER_LEFT) > (PLATFORM_BOTTOM - PLAYER_TOP)){
                        // replace on bottom
                        player.setPosition(PLAYER_LEFT, PLATFORM_BOTTOM);
                        if(velocity_y > 0.f) velocity_y = 0.f;
                    } else {
                        // replace on right
                        player.setPosition(PLATFORM_RIGHT, PLAYER_TOP);
                    }
                }
            }
            // MIDDLE ?
            else if(PLAYER_LEFT >= PLATFORM_LEFT && PLAYER_RIGHT <= PLATFORM_RIGHT) {
                // Top ?
                if(PLAYER_TOP <= PLATFORM_TOP && PLAYER_BOTTOM >= PLATFORM_TOP){
                    // replace on top
                    player.setPosition(PLAYER_LEFT, PLATFORM_TOP - player_box.height);
                    is_jumping = false;
                    velocity_y = 0.f;
                }
                // Bottom ?
                if(PLAYER_TOP <= PLATFORM_BOTTOM && PLAYER_BOTTOM >= PLATFORM_BOTTOM){
                    player.setPosition(PLAYER_LEFT, PLATFORM_BOTTOM);
                    if(velocity_y > 0.f) velocity_y = 0.f;
                }
            }
        }

        //// DRAW
        window.clear();
        window.draw(player);
        window.draw(platform);
        window.draw(platform2);
        window.display();
    }

    return 0;
}
