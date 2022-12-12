#include <iostream>

#include "Boids.h"

int main()
{
    Vivium::Application::Init(800, 600, 144);

    World world{};

    while (Vivium::Application::IsRunning()) {
        Vivium::Application::BeginFrame();

        world.Render();

        Vivium::Application::EndFrame();
    }

    Vivium::Application::Terminate();
}