#ifndef PSPRITE_H_INCLUDED
#define PSPRITE_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "U_2DBody.h"

class PSprite
{
public:
    PSprite(sf::Sprite sprite, U_2DBody *pin1, U_2DBody *pin2);

    sf::Sprite& getSprite() {return m_sprite;}
    void update();

    void draw(sf::RenderWindow &window);

private:
    sf::Sprite m_sprite;
    U_2DBody *_pin1, *_pin2;
    double m_initial_length, m_scale;

};

#endif // PSPRITE_H_INCLUDED
