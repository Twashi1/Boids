#include "Application.h"
#include "Input.h"
#include "Sprite.h"
#include "Panel.h"
#include "Slider.h"
#include "Text.h"
#include "TextInput.h"
#include "Button.h"
#include "Events.h"

namespace Vivium {
    Timer Application::m_Timer = Timer();

    int Application::m_FPS = 0;
    double Application::m_TimePerFrame = 0.0;
    float Application::m_Volume = 1.0f;

    std::unordered_map<Application::STATS_INDEX, std::shared_ptr<Text>> Application::m_StatsTextMap{};
    std::unordered_map<Application::CURSOR_TYPE, GLFWcursor*> Application::m_CursorMap{};

    unsigned int Application::m_FramesProcessed = 0;
    double Application::m_ProcessingTime = 0.0;

    VersionNumber Application::m_VersionNumber = "v0.0.0";
    std::shared_ptr<TextureAtlas> Application::engine_icons = nullptr;

    GLFWwindow* Application::window = nullptr;
    int Application::width = 0;
    int Application::height = 0;

    irrklang::ISoundEngine* Application::sound_engine = nullptr;

    std::string Application::resources_path = "../Resources/";

    std::shared_ptr<Panel> Application::window_panel = nullptr;

    void Application::m_WindowResizeCallback(GLFWwindow* window, int nwidth, int nheight)
    {
        VIVIUM_SCOPE;

        // Update window dimensions
        width = nwidth; height = nheight;

        Renderer::m_ResizeFramebuffers(width, height);

        // Update projection matrix for camera
        Renderer::camera->SetProj(0.0f, (float)width, 0.0f, (float)height);

        // TODO: update window panel
        window_panel->SetRect({ width * 0.5f, height * 0.5f, (float)width, (float)height });

        // Update viewport
        glViewport(0, 0, width, height);

        LogTrace("Resized to [{}, {}]", width, height);
    }

    void Application::BeginFrame()
    {
        VIVIUM_SCOPE;

        // Record time of frame beginning
        m_Timer.Start();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Update objects fixed to window panel
        window_panel->Update();

        // Update Input
        Input::Update();

        // Update physics
        Renderer::m_BeginScene(Renderer::PHYSICS_DEBUG_SCENE);
        Physics::Update();
        Renderer::EndScene();

        // Update vivium events system
        EventSystem::m_Update();

        // Poll and process events
        glfwPollEvents();
    }

    void Application::EndFrame()
    {
        VIVIUM_SCOPE;

        if (Flag::Test(VIVIUM_FLAG_DRAW_PHYSICS_CIRCLES)) {
            Renderer::DrawScene(Renderer::PHYSICS_DEBUG_SCENE);
        }

        // Update the screen from all the draw calls
        glFlush();
        glfwSwapBuffers(window);

        TextInput::m_ResetTypingStatus();
        Input::m_ResetScroll();

        double elapsed = m_Timer.GetElapsed();

        // Update performance tracking variables and display warning if running behind
        m_CalculatePerformance(elapsed);

        long double sleep_time = m_TimePerFrame - elapsed;
        double slept_for = 0.0;

        while (slept_for < sleep_time) {
            slept_for += m_Timer.GetElapsed();
        }
    }

    void Application::SetCursor(const CURSOR_TYPE& cursor)
    {
        glfwSetCursor(window, m_CursorMap.at(cursor));
    }

    void Application::SetFPS(int new_fps)
    {
        m_FPS = new_fps;
        m_TimePerFrame = 1.0f / m_FPS;
    }

    void Application::SetVolume(float volume)
    {
        m_Volume = std::min(std::max(volume, 0.0f), 1.0f); // Clamp to range 0.0f -> 1.0f
        sound_engine->setSoundVolume(m_Volume);
    }

    void Application::EnableWireframe()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void Application::DisableWireframe()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Vector2<int> Application::GetScreenDim()
    {
        return Vector2<int>(width, height);
    }

    bool Application::IsRunning()
    {
        return !glfwWindowShouldClose(window);
    }

    void Application::SetBGColor(const RGBColor& color)
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
    }

    void Application::Init(int nwidth, int nheight, int nfps, const char* nresources_path)
    {
        VIVIUM_SCOPE;

        Application::width = nwidth;
        Application::height = nheight;
        Application::m_FPS = nfps; Application::m_TimePerFrame = 1.0 / (double)m_FPS;

        sound_engine = irrklang::createIrrKlangDevice();

        if (!sound_engine) {
            LogFatal("Error initialising sound engine");
        }

        LogInfo("Application starting");

        std::regex path_cleaner("(\\/|(\\\\){2})$");

        resources_path = std::regex_replace(nresources_path, path_cleaner, "") + "/";

        // Set texture statics
        Texture::PATH = resources_path + "sprites/";

        // Set font statics
        Font::PATH = resources_path + "fonts/";

        // Make stb_image flip all images vertically so textures display correct way up
        stbi_set_flip_vertically_on_load(1);

        // Initialise glfw library, if it doesn't succeed, exit program
        if (!glfwInit()) { LogFatal("Couldn't initialise GLFW library"); }

        // Create window
        std::string title = std::format("NEA Game {}", m_VersionNumber);

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
        
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!window) {
            // Terminate glfw if window couldn't be created
            glfwTerminate();
            LogFatal("Window couldn't be created");
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Initialise glew library, and store any error returned to "err"
        GLenum err = glewInit();

        // If there was an error, get the error string and print
        if (err != GLEW_OK) {
            std::cout << "Glew error: " << glewGetErrorString(err) << std::endl;
            // Exit program
            exit(EXIT_FAILURE);
        }

        // Set background color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Allow transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Initialise statics for Quad
        Quad::m_Init();

        Physics::m_Init();

        // Set renderer's camera
        Renderer::m_Init();

        // Create icons texture
        engine_icons = std::make_shared<TextureAtlas>("engine_icons.png", Vivium::Vector2<int>(16, 16));

        // Initialise input system
        Input::m_Init();

        Sprite::m_Init();

        // Initialise static for Animation
        Animation::FILE_EXTENSION = ".animation";

        // Initialise text class
        Text::m_Init();

        // Initialise button class
        Button::m_Init();

        // Initialise slider class
        Slider::m_Init();

        // Initialise text input class
        TextInput::m_Init(engine_icons);

        // Initialise seed for random class
        Random::m_Init();

        // TODO Panel::Init(width, height)
        // Construct window panel
        std::shared_ptr<Quad> panel_quad = std::make_shared<Quad>(width * 0.5f, height * 0.5f, (float)width, (float)height);
        window_panel = std::make_shared<Panel>(panel_quad);

        // Setup message callbacks
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(__GLLogCallback, NULL);

        // Setup window resize callback
        glfwSetWindowSizeCallback(window, m_WindowResizeCallback);

        // Allow transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Create all text renderables for stats
        m_StatsTextMap.insert({ STATS_INDEX::AVERAGE_TPF,            std::make_shared<Text>("", Vector2<float>(5, -15), Text::Alignment::LEFT, 0.25)  });
        m_StatsTextMap.insert({ STATS_INDEX::PROCESSING_PERCENTAGE,  std::make_shared<Text>("", Vector2<float>(5, -30), Text::Alignment::LEFT, 0.25)  });
        m_StatsTextMap.insert({ STATS_INDEX::FPS,                    std::make_shared<Text>("", Vector2<float>(5, -45), Text::Alignment::LEFT, 0.25)  });
        m_StatsTextMap.insert({ STATS_INDEX::PLAYER_POS,             std::make_shared<Text>("", Vector2<float>(5, -100), Text::Alignment::LEFT, 0.25) });

        // Anchor the text objects
        for (auto& [index, text] : m_StatsTextMap) {
            window_panel->Anchor(Panel::ANCHOR::LEFT, Panel::ANCHOR::TOP, text);
        }

        // Initialise cursor objects
        for (int i = (int)CURSOR_TYPE::__START; i < (int)CURSOR_TYPE::__MAX; i++) {
            m_CursorMap.insert(
                {(CURSOR_TYPE)i, glfwCreateStandardCursor(i)}
            );
        }

        // Finished loading
        LogInfo("Loaded engine on version: {}", m_VersionNumber);
    }

    void Application::Terminate()
    {
        VIVIUM_SCOPE;

        LogInfo("Application shutting down");

        Quad::m_Terminate();

        sound_engine->drop();

        // NOTE: takes cares of the cursors we create as well
        glfwTerminate();
    }

    void Application::m_CalculatePerformance(double dt)
    {
        VIVIUM_SCOPE;

        // Update performance tracking variables
        m_FramesProcessed++;
        m_ProcessingTime += dt;

        // If POLL_INTERVAL has passed
        if (m_FramesProcessed * m_TimePerFrame > POLL_INTERVAL) {
            double average_tpf = m_ProcessingTime / m_FramesProcessed; // Calculate average processing time taken per frame

            // If average time per frame is more than max time per frame, we're running behind so display a message
            if (average_tpf > m_TimePerFrame) {
                LogWarn("Running behind by {:.3f}ms", Math::Round((average_tpf - m_TimePerFrame) * 1000, 3));
            }

            // Reset trackers
            m_FramesProcessed = 0;
            m_ProcessingTime = 0;
        }
    }

    void Application::UpdateStats(const Body& player_body)
    {
        VIVIUM_SCOPE;

        {
            double avg_tpf = (m_ProcessingTime / m_FramesProcessed);
            double avg_tpf_ms = Math::Round(avg_tpf * 1000.0, 3);

            RGBColor color = RGBColor::WHITE;
            if (avg_tpf > m_TimePerFrame) { color = RGBColor::RED; }
            else if (avg_tpf > m_TimePerFrame * 0.7) { color = RGBColor::YELLOW; }

            std::string text = std::format("Average time per frame: {:.3f}ms/{:.3f}ms", avg_tpf_ms, m_TimePerFrame * 1000);

            std::shared_ptr<Text> t = m_StatsTextMap[STATS_INDEX::AVERAGE_TPF];
            t->shader->Bind(); t->shader->SetUniform3f("u_TextColor", color.r, color.g, color.b);
            t->SetText(text);

            Renderer::Submit(t.get());
        }

        {
            double percentage_processing = Math::Round((m_ProcessingTime / (m_TimePerFrame * m_FramesProcessed)) * 100.0f, 2);

            RGBColor color = RGBColor::WHITE;
            if (percentage_processing > 100) { color = RGBColor::RED; }
            else if (percentage_processing > 70) { color = RGBColor::YELLOW; }

            std::string text = std::format("Processing time: {:.3f}%", percentage_processing);

            std::shared_ptr<Text> t = m_StatsTextMap[STATS_INDEX::PROCESSING_PERCENTAGE];
            t->shader->Bind(); t->shader->SetUniform3f("u_TextColor", color.r, color.g, color.b);
            t->SetText(text);

            Renderer::Submit(t.get());
        }

        {
            float fps = POLL_INTERVAL / (m_ProcessingTime / m_FramesProcessed);

            RGBColor color = RGBColor::WHITE;
            std::string text;

            if (fps < m_FPS * 0.7f) {
                color = RGBColor::RED;
                text = std::format("FPS: {:.3f}", fps);
            }
            else if (fps < m_FPS) {
                color = RGBColor::YELLOW;
                text = std::format("Max FPS: {:.3f}; Actual FPS: {}", fps, fps);
            }
            else {
                text = std::format("Max FPS: {:.3f}; Actual FPS: {}", fps, m_FPS);
            }

            std::shared_ptr<Text> t = m_StatsTextMap[STATS_INDEX::FPS];
            t->shader->Bind(); t->shader->SetUniform3f("u_TextColor", color.r, color.g, color.b);
            t->SetText(text);

            Renderer::Submit(t.get());
        }

        {
            Vector2<float> player_position = player_body.quad->GetCenter();

            std::string text = std::format("Player position: [{:.3f}, {:.3f}]", player_position.x, player_position.y);

            std::shared_ptr<Text> t = m_StatsTextMap[STATS_INDEX::PLAYER_POS];
            t->shader->Bind(); t->shader->SetUniform3f("u_TextColor", RGBColor::WHITE);
            t->SetText(text);

            Renderer::Submit(t.get());
        }
    }

    void Application::StartSound(const std::string& file)
    {
        std::string path = resources_path + "sounds/" + file;
        sound_engine->play2D(path.c_str());
    }
}