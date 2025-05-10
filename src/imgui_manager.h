#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>

namespace Eon
{
    class ImGuiManager
    {
    public:
        explicit ImGuiManager(sf::Window& window);
        ~ImGuiManager();

        void Init();
        void ProcessEvent(const sf::Event& event) const;
        void Update(float deltaTime);
        void Render() const;

        void SetRenderFunction(std::function<void()> renderFunction);

    private:
        sf::Window& window;
        sf::Clock imgui_clock;
        std::function<void()> render_function;
        bool initialized;
    };
}