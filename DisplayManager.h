#ifndef DISPLAYMANAGER_H_INCLUDED
#define DISPLAYMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "UnitedEngine.h"
#include "PSprite.h"

class DisplayManager
{
public:
    DisplayManager(sf::RenderWindow *window, U_2DCollisionManager *collisionManager);

    //offset mutators
    void setOffset(double x, double y) {m_offsetX=x; m_offsetY=y;};
    void setOffset(U_2DCoord off) {m_offsetX=off.x; m_offsetY=off.y;};

    void addOffset(double x, double y) {m_offsetX+=x/m_zoom; m_offsetY+=y/m_zoom;};
    void addOffset(U_2DCoord off) {m_offsetX+=off.x/m_zoom; m_offsetY+=off.y/m_zoom;};

    // add psprite
    void addPSprite(PSprite ps) {_psprites.push_back(ps);}

    // set the absolute zoom
    void setZoom(double zoom) {m_zoom = zoom;};

    // zoom on the current view (depending on the current zoom)
    void zoom(double zoomFactor) {m_zoom *= zoomFactor;};

    // draw the current gameWorld
    void draw(bool showInner);

    // getters
    U_2DCoord getOffset() const {return U_2DCoord(m_offsetX, m_offsetY);};
    double getZoom() const {return m_zoom;};
    U_2DCoord worldCoordToDisplayCoord(const U_2DCoord&);
    U_2DCoord displayCoordToWorldCoord(const U_2DCoord&);
    U_2DBody* getBodyAt(double x, double y);

private:
    U_2DCollisionManager* m_collisionManager;
    sf::RenderWindow* m_window;
    std::vector<PSprite> _psprites;
    sf::Texture m_bodyTexture;

    double m_zoom, m_offsetX, m_offsetY, m_windowOffsetX, m_windowOffsetY;
};

#endif // DISPLAYMANAGER_H_INCLUDED
