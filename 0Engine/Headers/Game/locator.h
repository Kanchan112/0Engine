#pragma once
#include "s00nya_defs.h"

namespace s00nya
{

	class Debug;
	class Window;
	class Timer;
	class Input;
	class InputManager;
	class Resources;
	class Shader;
	class Renderer;
	class Locator;

	class S00NYA_API Locator
	{
	private:
		static Locator* instance;

	private:
		Locator();
		~Locator();

	public:
		Window* WindowService(const Character* title, const Integer& width, const Integer& height) const;
		Timer* TimerService() const;
		Input* InputService(const Window* window) const;
		InputManager* InputManagerService(Input* input) const;
		Resources* ResourceService() const;
		Shader* ShaderService(const Character* path) const;
		Renderer* RendererService() const;

		static Locator& Get();
	};

}