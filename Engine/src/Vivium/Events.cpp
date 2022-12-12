#include "Events.h"

namespace Vivium {
	std::vector<std::shared_ptr<EventHandler>> EventSystem::m_EventHandlers;

	void EventHandler::m_AddEvent(std::shared_ptr<Event> event)
	{
		event->m_InvokeTimer.Start();
		m_Events.push(event);
	}

	void EventHandler::Update()
	{
		VIVIUM_SCOPE;

		while (!m_Events.empty()) {
			// Handle first event
			m_HandleEvent(m_Events.front());
			// Pop event that's been handled
			m_Events.pop();
		}
	}
	
	EventHandler::EventHandler(const std::string& type) : m_HandlingType(type), m_Events() {}
	 
	Event::Event(const std::string& type) : type(type) {}

	void EventSystem::m_Update()
	{
		VIVIUM_SCOPE;

		if (!m_EventHandlers.empty()) {
			for (auto event_handler : m_EventHandlers) {
				event_handler->Update();
			}
		}
	}

	void EventSystem::AddEvent(std::shared_ptr<Event> event)
	{
		VIVIUM_SCOPE;

		if (!m_EventHandlers.empty()) {
			for (auto& handler : m_EventHandlers) {
				if (handler->m_HandlingType == event->type) {
					handler->m_AddEvent(event);
				}
			}
		}
	}

	void EventSystem::RegisterHandler(std::shared_ptr<EventHandler> handler)
	{
		m_EventHandlers.push_back(handler);
	}

	const std::vector<std::shared_ptr<EventHandler>> EventSystem::GetHandlers()
	{
		return m_EventHandlers;
	}

	void EventSystem::ForceUpdate()
	{
		m_Update();
	}
}
