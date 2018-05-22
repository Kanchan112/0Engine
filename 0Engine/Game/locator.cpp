#include "Game/locator.h"
#include "Debugger/logger.h"
#include "Graphics/window.h"
#include "Utility/timer.h"
#include "Input/input.h"
#include "Input/input_manager.h"

namespace s00nya
{

	Locator* Locator::instance(nullptr);

	Locator::Locator()
	{
	}

	Locator::~Locator()
	{
		if (instance)
		{
			Debug::ShutDown();
			delete instance;
		}
	}

	Window* Locator::GetWindow(const Character* title, const Integer& width, const Integer& height) const
	{
		Window* temp = new Window(title, width, height);
		Debug::Initialize();
		return temp;
	}

	Timer* s00nya::Locator::GetTimer() const
	{
		return new Timer();
	}

	Input* Locator::GetInput(const Window* window) const
	{
		return new Input(window->m_id);
	}

	InputManager* Locator::GetInputManager(Input* input) const
	{
		return new InputManager(input);
	}

	Locator& Locator::Get()
	{
		if (instance) return *instance;
		instance = new Locator();
		return *instance;
	}

}