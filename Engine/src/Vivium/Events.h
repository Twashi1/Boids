#pragma once

#include "Core.h"
#include "Timer.h"
#include "Application.h"

namespace Vivium {
	class EventSystem;
	class EventHandler;

	class VIVIUM_API Event {
	protected:
		// Time since event invoked
		Timer m_InvokeTimer;

	public:
		// TODO: use some event ID
		std::string type;

		Event(const std::string& type);
		virtual ~Event() = default;

		friend EventSystem;
		friend EventHandler;
	};

	class VIVIUM_API EventHandler {
	private:
		void m_AddEvent(std::shared_ptr<Event> event);

	protected:
		std::string m_HandlingType;
		std::queue<std::shared_ptr<Event>> m_Events = {};

		virtual void m_HandleEvent(std::shared_ptr<Event> event) = 0;

	public:
		void Update();

		EventHandler(const std::string& type);
		virtual ~EventHandler() = default;

		friend EventSystem;
	};

	class VIVIUM_API EventSystem {
	private:
		static std::vector<std::shared_ptr<EventHandler>> m_EventHandlers;
		static void m_Update();

	public:
		static void AddEvent(std::shared_ptr<Event> event);
		static void RegisterHandler(std::shared_ptr<EventHandler> handler);
		static const std::vector<std::shared_ptr<EventHandler>> GetHandlers();

		static void ForceUpdate();

		friend Application;
	};
}
