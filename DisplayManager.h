#ifndef DISPLAYMANAGER_H_INCLUDED
#define DISPLAYMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "united_solver.h"

class DisplayManager
{
public:
    DisplayManager(sf::RenderWindow *window, UnitedSolver *collisionManager);

    //offset mutators
    void setOffset(double x, double y) {m_offsetX=x; m_offsetY=y;};
    void setOffset(const Vec2& off) {m_offsetX=off.x; m_offsetY=off.y;};

    void addOffset(double x, double y) {m_offsetX+=x/m_zoom; m_offsetY+=y/m_zoom;};
    void addOffset(const Vec2& off) {m_offsetX+=off.x/m_zoom; m_offsetY+=off.y/m_zoom;};

    // set the absolute zoom
    void setZoom(double zoom) {m_zoom = zoom;};

    // zoom on the current view (depending on the current zoom)
    void zoom(double zoomFactor) {m_zoom *= zoomFactor;};

    // draw the current gameWorld
    void draw(bool showInner);

    // getters
    Vec2 getOffset() const {return Vec2(m_offsetX, m_offsetY);};
    double getZoom() const {return m_zoom;};
    Vec2 worldCoordToDisplayCoord(const Vec2&);
	Vec2 displayCoordToWorldCoord(const Vec2&);
    Body* getBodyAt(double x, double y);

private:
    UnitedSolver* m_collisionManager;
    sf::RenderWindow* m_window;
    sf::Texture m_bodyTexture;

    double m_zoom, m_offsetX, m_offsetY, m_windowOffsetX, m_windowOffsetY;
};

#endif // DISPLAYMANAGER_H_INCLUDED
