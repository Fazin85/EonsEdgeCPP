#include "imgui_manager.h"
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "log.h"
#include <SFML/OpenGL.hpp>

namespace Eon
{
	ImGuiManager::ImGuiManager(sf::Window& window)
		: window(window), initialized(false)
	{
	}

	ImGuiManager::~ImGuiManager()
	{
		if (initialized)
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext();
		}
	}

	void ImGuiManager::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Initialize OpenGL3 backend
		ImGui_ImplOpenGL3_Init("#version 330");

		// Setup display size (assuming window has been created already)
		io.DisplaySize = ImVec2(static_cast<float>(window.getSize().x),
			static_cast<float>(window.getSize().y));

		initialized = true;
		EON_INFO("ImGui initialized successfully");
	}

	void ImGuiManager::ProcessEvent(const sf::Event& event) const
	{
		if (!initialized) return;

		ImGuiIO& io = ImGui::GetIO();

		// Process keyboard events
		if (event.type == sf::Event::KeyPressed)
			io.KeysDown[event.key.code] = true;
		if (event.type == sf::Event::KeyReleased)
			io.KeysDown[event.key.code] = false;

		// Process text input
		if (event.type == sf::Event::TextEntered)
			io.AddInputCharacter(event.text.unicode);

		// Process mouse events
		if (event.type == sf::Event::MouseButtonPressed)
			io.MouseDown[event.mouseButton.button] = true;
		if (event.type == sf::Event::MouseButtonReleased)
			io.MouseDown[event.mouseButton.button] = false;
		if (event.type == sf::Event::MouseWheelScrolled)
			io.MouseWheel += event.mouseWheelScroll.delta;
		if (event.type == sf::Event::MouseMoved)
			io.MousePos = ImVec2(static_cast<float>(event.mouseMove.x),
				static_cast<float>(event.mouseMove.y));

		// Handle window resize
		if (event.type == sf::Event::Resized)
			io.DisplaySize = ImVec2(static_cast<float>(event.size.width),
				static_cast<float>(event.size.height));
	}

	void ImGuiManager::Update(float deltaTime)
	{
		if (!initialized) return;

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = deltaTime;

		// Update mouse position
		if (window.hasFocus())
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			io.MousePos = ImVec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		}

		// Update special keys
		io.KeyCtrl = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
		io.KeyShift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		io.KeyAlt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		if (render_function)
		{
			render_function();
		}

		ImGui::Render();
	}

	void ImGuiManager::Render() const
	{
		if (!initialized) return;
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::SetRenderFunction(std::function<void()> function)
	{
		render_function = function;
	}
}