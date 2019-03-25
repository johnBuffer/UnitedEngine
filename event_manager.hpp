#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>

class EventManager
{
public:
	EventManager(sf::Window& window) :
		m_window(window)
	{}

	void registerCallback(sf::Event::EventType type, std::function<void(void)> function)
	{
		m_callmap[type] = function;
	}

	void processEvents() const
	{
		// Iterate over events
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			// If event type is registred
			sf::Event::EventType type = event.type;
			auto it(m_callmap.find(type));
			if (it != m_callmap.end())
			{
				// Call its associated callback
				(it->second)();
			}
		}
	}

	void removeCallback(sf::Event::EventType type)
	{
		// If event type is registred
		auto it(m_callmap.find(type));
		if (it != m_callmap.end())
		{
			// Remove its associated callback
			m_callmap.erase(it);
		}
	}

private:
	sf::Window& m_window;

	std::unordered_map<sf::Event::EventType, std::function<void(void)>> m_callmap;

};