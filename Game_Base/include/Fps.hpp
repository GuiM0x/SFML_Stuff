#ifndef FPS_HPP_INCLUDED
#define FPS_HPP_INCLUDED

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>
#include <string>

class Fps : public sf::Drawable
{
public:
	Fps() = default;
	Fps(const sf::Font& font, unsigned characterSize = 16) :
		m_fpsText{"0", font, characterSize}
	{}

public:
	void setFont(const sf::Font& font, unsigned characterSize = 16){
		m_fpsText = sf::Text{"0", font, characterSize};
	}

    void update(const sf::Time& dt){
        m_elapsed += dt.asSeconds();
        if(m_elapsed>m_timeToUpdateString){
            m_elapsed = 0.f;
            float fps{1.f/dt.asSeconds()};
            m_fpsText.setString(std::to_string(static_cast<unsigned>(fps)));
        }
    }

    void setTextRefreshTime(float time_in_seconds){
        m_timeToUpdateString = time_in_seconds;
    }

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const final override{
		target.draw(m_fpsText, states);
	}

private:
    float m_timeToUpdateString{0.1f};
    float m_elapsed{0.f};
	sf::Text m_fpsText{};
};

#endif // FPS_HPP_INCLUDED
