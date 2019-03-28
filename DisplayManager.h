#ifndef DISPLAYMANAGER_H_INCLUDED
#define DISPLAYMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "united_solver.h"
#include "event_manager.hpp"

class DisplayManager
{
public:
    DisplayManager(sf::RenderWindow *window, up::UnitedSolver *collisionManager);

    //offset mutators
    void setOffset(double x, double y) {m_offsetX=x; m_offsetY=y;};
    void setOffset(const up::Vec2& off) {m_offsetX=off.x; m_offsetY=off.y;};

    void addOffset(double x, double y) {m_offsetX-=x/m_zoom; m_offsetY-=y/m_zoom;};
    void addOffset(const up::Vec2& off) {m_offsetX-=off.x/m_zoom; m_offsetY-=off.y/m_zoom;};

    // set the absolute zoom
    void setZoom(double zoom) {m_zoom = zoom;};

    // zoom on the current view (depending on the current zoom)
    void zoom(double zoomFactor) {m_zoom *= zoomFactor;};

    // draw the current gameWorld
    void draw(bool showInner);

	void processEvents();

    // getters
    up::Vec2 getOffset() const {return up::Vec2(m_offsetX, m_offsetY);};
    double getZoom() const {return m_zoom;};
	up::Vec2 worldCoordToDisplayCoord(const up::Vec2&);
	up::Vec2 displayCoordToWorldCoord(const up::Vec2&);
	up::Body* getBodyAt(double x, double y);

	// Draw constraints
	void drawConstraints(const fva::SwapArray<up::Constraint>& constraints);
	void drawMuscles(const fva::SwapArray<up::Muscle>& constraints);

	bool emit;
	float render_time;

	sfev::EventManager& eventManager()
	{
		return m_event_manager;
	}

private:
	up::UnitedSolver* m_collisionManager;
    sf::RenderWindow* m_window;
    sf::Texture m_bodyTexture;

	sfev::EventManager m_event_manager;

	bool _mouse_button_pressed;
	sf::Vector2i _drag_clic_position, _clic_position;

    double m_zoom, m_offsetX, m_offsetY, m_windowOffsetX, m_windowOffsetY;

	bool m_show_pressure;
};

#endif // DISPLAYMANAGER_H_INCLUDED
