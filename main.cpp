#include <cstdlib>
#include <iostream>

#include "UnitedEngine.h"
#include "DisplayManager.h"
#include "PSprite.h"

#include <SFML/Graphics.hpp>
#include <thread>

int main(int argn, char* args[])
{
    int window_width = 1600;
    int window_height = 900;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "TEST", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    int bodyRadius = 20;
    int mapSizeX = 10000;
    int mapSizeY = 10000;

    U_2DCollisionManager phyManager(0.016, bodyRadius, U_2DCoord(mapSizeX, mapSizeY));
    phyManager.setPrecision(1);
    phyManager.setGravity(U_2DCoord(0, 10));
    DisplayManager displayManager(&window, &phyManager);
    displayManager.setOffset(-mapSizeX/2, -0.75*mapSizeY);
    displayManager.setZoom(0.5);

    sf::Clock clock;
    bool mouseButtonPressed = false;
    sf::Vector2i dragClicPosition, clicPosition;
    //int n=0;

    sf::Texture characterTexture;
    characterTexture.loadFromFile("robot2.png");
    sf::Sprite head, arm1, arm2, hand1, hand2, leg1, leg2, foot1, foot2, body;
    head.setTexture(characterTexture);
    head.setTextureRect(sf::IntRect(5, 3, 51, 57));

    body.setTexture(characterTexture);
    body.setTextureRect(sf::IntRect(126, 51, 45, 72));

    arm1.setTexture(characterTexture);
    arm1.setTextureRect(sf::IntRect(101, 59, 18, 35));
    hand1.setTexture(characterTexture);
    hand1.setTextureRect(sf::IntRect(99, 98, 25, 67));

    arm2.setTexture(characterTexture);
    arm2.setTextureRect(sf::IntRect(21, 166, 15, 32));
    hand2.setTexture(characterTexture);
    hand2.setTextureRect(sf::IntRect(65, 166, 19, 66));

    leg1.setTexture(characterTexture);
    leg1.setTextureRect(sf::IntRect(117, 130, 20, 48));
    foot1.setTexture(characterTexture);
    foot1.setTextureRect(sf::IntRect(106, 182, 30, 71));

    leg2.setTexture(characterTexture);
    leg2.setTextureRect(sf::IntRect(164, 130, 20, 48));
    foot2.setTexture(characterTexture);
    foot2.setTextureRect(sf::IntRect(165, 182, 30, 71));

    U_2DBody* selectedBody = NULL;
    bool upPhy = false;
    bool soft = false;

    /*U_2DBody* b1 = phyManager.addBody(U_2DCoord(430, 480), 0, 1);
    U_2DBody* b2 = phyManager.addBody(U_2DCoord(465, 520), 1);
    U_2DBody* b3 = phyManager.addBody(U_2DCoord(400, 520), 1);
    U_2DBody* b4 = phyManager.addBody(U_2DCoord(300, 520), 1);
    U_2DBody* b5 = phyManager.addBody(U_2DCoord(370, 480), 0, 1);

    b1->setStatic(false);
    b2->setStatic(true);

    phyManager.addConstraint(b1, b3);
    phyManager.addConstraint(b2, b3);
    phyManager.addConstraint(b2, b1);
    phyManager.addConstraint(b3, b4);
    phyManager.addConstraint(b5, b4);
    phyManager.addConstraint(b5, b3);
    U_2DConstraint* c1 = phyManager.addConstraint(b5, b1);
    c1->setOnlyTension(true);

    displayManager.addPSprite(PSprite(arm1, b2, b3));
    displayManager.addPSprite(PSprite(hand1, b3, b4));*/

    U_2DRigidBody tower(20, 80);
    //phyManager.addRigidBody(tower, U_2DCoord(1000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    //phyManager.addRigidBody(tower, U_2DCoord(2000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    phyManager.addRigidBody(tower, U_2DCoord(3000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    phyManager.addRigidBody(tower, U_2DCoord(4000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    phyManager.addRigidBody(tower, U_2DCoord(5000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    phyManager.addRigidBody(tower, U_2DCoord(6000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    //phyManager.addRigidBody(tower, U_2DCoord(7000, 10000-bodyRadius*2*tower.getHeight()), 1.2);
    //phyManager.addRigidBody(tower, U_2DCoord(8000, 10000-bodyRadius*2*tower.getHeight()), 1.2);

    while (window.isOpen())
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event))
        {
			switch (event.type)
			{
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape) window.close();
					else if ((event.key.code == sf::Keyboard::Subtract)) displayManager.zoom(0.8);
					else if ((event.key.code == sf::Keyboard::Add)) displayManager.zoom(1.2);
					else if ((event.key.code == sf::Keyboard::Space)) upPhy = !upPhy;
					else if ((event.key.code == sf::Keyboard::A))
                    {
                        soft = !soft;
                    }
                    break;
				case sf::Event::MouseWheelMoved:
				    // this is an amazing zoom
					displayManager.zoom(1+event.mouseWheel.delta/5.0);
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        mouseButtonPressed = true;
                        dragClicPosition = mousePosition;
                        clicPosition = mousePosition;

                        selectedBody = displayManager.getBodyAt(mousePosition.x, mousePosition.y);
                    }
                    else if (selectedBody)
                    {
                        selectedBody->setStatic(!selectedBody->isStatic());
                    }

                    break;
                case sf::Event::MouseButtonReleased:
                    mouseButtonPressed = false;
                    if (clicPosition == mousePosition)
                    {
                        for (int i(5); --i;)
                        {
                            U_2DCoord pos(mousePosition.x+rand()%10, mousePosition.y+rand()%10);
                            U_2DCoord worldPos = displayManager.displayCoordToWorldCoord(pos);
                            phyManager.addBody(worldPos, 20);
                        }
                    }

                    selectedBody = NULL;

                    break;
                case sf::Event::MouseMoved:
                    if (mouseButtonPressed && selectedBody == NULL) // in this case we are dragging
                    {
                        // updating displayManager offset
                        displayManager.addOffset(mousePosition.x-dragClicPosition.x, mousePosition.y-dragClicPosition.y);
                        dragClicPosition = mousePosition;
                    }
                    break;
				default:
                    break;
			}
        }

        if (selectedBody)
        {
            U_2DCoord mouseWorldPos = displayManager.displayCoordToWorldCoord(U_2DCoord(mousePosition.x, mousePosition.y));
            U_2DCoord f = mouseWorldPos - selectedBody->getPosition();
            selectedBody->move2D(U_2DCoord(0.01*f.x, 0.01*f.y));
        }

        if (upPhy)
            phyManager.update();

        window.clear(sf::Color::White);

        displayManager.draw(false);

        window.display();

    }

    std::cout<<"All working fine so far !"<<std::endl;

    return 0;
}
