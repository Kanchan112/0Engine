#include "Game/game.h"
#include "Graphics/window.h"
#include "Debugger/logger.h"
#include "Utility/timer.h"
#include "Input/input.h"
#include "Input/input_manager.h"
#include "Utility/resource_manager.h"
#include "Game/locator.h"
#include "Physics/collider_sat.h"
#include "Physics/collision_sat.h"
#include "GameObject/scene_2d.h"
#include "GameObject/game_object_2d.h"
#include "Graphics/renderer.h"
#include "GL/glew.h"

namespace s00nya
{

	Game2D::Game2D(const Character* title, const Integer& width, const Integer& height) :
		window(Locator::Get().WindowService(title, width, height)),
		timer(Locator::Get().TimerService()),
		input(Locator::Get().InputService(window)),
		inputManager(Locator::Get().InputManagerService(input)),
		resource(Locator::Get().ResourceService()),
		renderer(Locator::Get().RendererService())
	{
		m_shaders["Default2DShader"] = Locator::Get().ShaderService("./Resources/Default2DShader.glsl");
		instance = this;
	}

	Game2D::~Game2D()
	{
		delete m_shaders["Default2DShader"];
		m_shaders.clear();

		for (auto& scene : m_scenes)
			delete scene;
		m_scenes.clear();

		delete resource;
		delete inputManager;
		delete input;
		delete timer;
		delete window;
	}

	void Game2D::Start()
	{
		window->Show();

		// Time Management
		Float now = Timer::ElaspedTime();
		Float deltaTimeForSecond = 0.0f;
		
		while (window->IsRunning())
		{
			// Runs every 1 second
			if (Timer::ElaspedTime() - now > 1.0f)
			{
				now = Timer::ElaspedTime();
				Tick();
			}

			// Runs 60 times a second
			if (deltaTimeForSecond * fps > 1.0f)
			{
				deltaTimeForSecond = 0.0f;
				FixedUpdate();
			}

			// As fast as possible
			Update();

			// Sum up delta time to get total time difference
			deltaTimeForSecond += timer->DeltaTime();

			timer->Update();
			window->Update();
		}
	}

	void Game2D::Tick()
	{
		Debug::Log(true);
		printf("\nFPS : %d", (Integer)(1.0f / timer->DeltaTime()));
	}

	void Game2D::FixedUpdate()
	{
		auto& objects(Locator::Get().GetAllObjects2D(m_scenes[m_activeScene]));
		for (auto* object : objects)
			object->FixedUpdate();
			
		//CollisionSAT::CollsionResolution(m_gameObjects);
	}

	void Game2D::Update()
	{
		auto& objects(Locator::Get().GetAllObjects2D(m_scenes[m_activeScene]));
		for (auto* object : objects)
			object->Update();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer->Initialize(*m_scenes[m_activeScene], Renderer::Type::GAME_OBJECT_2D, m_shaders["Default2DShader"]);
		for (auto* object : objects)
		{
			if(GetBIT(object->GetFlags(), 0))
				renderer->Draw(*object, resource->GetSpriteSheet(object->material.diffuse));
		}
	}

	Input& Game2D::GetInput()
	{
		return *(instance->input);
	}

	Timer& Game2D::GetTimer()
	{
		return *(instance->timer);
	}

	InputManager& Game2D::GetInputManager()
	{
		return *(instance->inputManager);
	}

	Resources& Game2D::GetResourceManager()
	{
		return *(instance->resource);
	}

	void Game2D::ActivateScene(const PDUInteger& id)
	{
		instance->m_activeScene = id;
	}

	void Game2D::ActivateNextScene()
	{
		instance->m_activeScene++;
	}

	PDUInteger Game2D::PushScene(Scene* scene)
	{
		instance->m_scenes.push_back(scene);
		return instance->m_scenes.size() - 1;
	}

	void Game2D::PopSceneBack()
	{
		delete (instance->m_scenes.back());
		instance->m_scenes.pop_back();
	}

	void Game2D::PopSceneFront()
	{
		delete (instance->m_scenes.front());
		instance->m_scenes.pop_front();
	}

	Scene& Game2D::GetCurrentScene()
	{
		return *instance->m_scenes[instance->m_activeScene];
	}

	void Game2D::AddGameObject2D(GameObject2D* object2D, const Character* name)
	{
		instance->m_scenes[instance->m_activeScene]->AddObject2D(object2D, name);
	}

	GameObject2D& Game2D::GetObject2D(const Character* name)
	{
		return instance->m_scenes[instance->m_activeScene]->GetObject2D(name);
	}

	void Game2D::RemoveObject2D(const Character* name)
	{
		instance->m_scenes[instance->m_activeScene]->RemoveObject2D(name);
	}

	const Float Game2D::fps = 60.0f;

	Game2D* Game2D::instance = nullptr;

}