#ifndef DISPLAYMANAGER_H_INCLUDED
#define DISPLAYMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "united_solver.hpp"
#include "event_manager.hpp"
#include "segment.hpp"

class DisplayManager
{
public:
    DisplayManager(sf::RenderTarget& window, sf::RenderWindow& ev_window, up::UnitedSolver& collisionManager);

    //offset mutators
    void setOffset(float x, float y) {m_offsetX=x; m_offsetY=y;};
    void setOffset(const up::Vec2& off) {m_offsetX=off.x; m_offsetY=off.y;};

    void addOffset(float x, float y) {m_offsetX-=x/m_zoom; m_offsetY-=y/m_zoom;};
    void addOffset(const up::Vec2& off) {m_offsetX-=off.x/m_zoom; m_offsetY-=off.y/m_zoom;};

    // set the absolute zoom
    void setZoom(float zoom) {m_zoom = zoom;};

    // zoom on the current view (depending on the current zoom)
    void zoom(float zoomFactor) {m_zoom *= zoomFactor;};

    // draw the current gameWorld
    void draw(bool showInner);

	void updateVertexArray(const std::vector<up::Body>& bodies, uint32_t id, uint32_t step);

	void processEvents();

    // getters
    up::Vec2 getOffset() const {return up::Vec2(m_offsetX, m_offsetY);};
    float getZoom() const {return m_zoom;};
	up::Vec2 worldCoordToDisplayCoord(const up::Vec2&);
	up::Vec2 displayCoordToWorldCoord(const up::Vec2&);
	up::Body* getBodyAt(float x, float y);

	// Draw constraints
	void drawConstraints(const fva::SwapArray<up::Constraint>& constraints);

	void drawPoint(const up::Vec2& point);
	void drawSegment(const up::SolidSegment& segment);

	bool clic;
	bool emit;
	bool update;
	float render_time;
	bool speed_mode;

	sfev::EventManager& eventManager()
	{
		return m_event_manager;
	}

	const up::Vec2& getClicPosition() const
	{
		return up::Vec2(m_clic_position.x, m_clic_position.y);
	}

private:
	up::UnitedSolver& m_solver;
	sf::RenderTarget& m_target;
	sf::RenderWindow& m_window;
    sf::Texture m_bodyTexture;

	sfev::EventManager m_event_manager;
	
	sf::VertexArray m_va;
	Swarm<up::Body> m_swarm;

	bool m_mouse_button_pressed;
	sf::Vector2i m_drag_clic_position, m_clic_position;

    float m_zoom, m_offsetX, m_offsetY, m_windowOffsetX, m_windowOffsetY;

	bool m_show_pressure;
};

#endif // DISPLAYMANAGER_H_INCLUDED
