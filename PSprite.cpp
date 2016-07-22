#include "PSprite.h"
#include <iostream>

PSprite::PSprite(sf::Sprite& sprite, U_2DBody* pin1, U_2DBody* pin2)
{
    m_sprite = sprite;

    _pin1 = pin1;
    _pin2 = pin2;

    m_initial_length = (_pin1->getPosition()-_pin2->getPosition()).getNorm();

    double l = m_sprite.getGlobalBounds().width;
    double h = m_sprite.getGlobalBounds().height;

    m_sprite.setOrigin(l/2, h/2);

    m_scale = 1.2*m_initial_length/h;

    std::cout << 1.2*m_initial_length/h << std::endl;
}

void PSprite::update()
{
    U_2DCoord c1 = _pin1->getPosition();
    U_2DCoord c2 = _pin2->getPosition();

    U_2DCoord v = c1 - c2;
    double length = v.getNorm();

    double angle = acos(v.x/length);
    if (v.y < 0)
        angle *= -1;

    m_sprite.setRotation(angle*57.2957795+90);

    //calcul de la position
    double x = (c1.x+c2.x)/2.0;
    double y = (c1.y+c2.y)/2.0;

    m_sprite.setPosition(x, y);
    m_sprite.setScale(m_scale, m_scale);
}

void PSprite::draw(sf::RenderWindow &window)
{
    //calcul de l'angle
    update();
    window.draw(m_sprite);
}
