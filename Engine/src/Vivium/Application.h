#pragma once

#include "Physics.h"
#include "Camera.h"
#include "VersionNumber.h"
#include "Animation.h"
#include "Math.h"
#include "Color.h"
#include "Renderer.h"
#include "Random.h"

namespace Vivium {
    class Panel;

    class VIVIUM_API Application {
    public:
        enum class CURSOR_TYPE : int {
            __START             = GLFW_ARROW_CURSOR,
            POINTER             = GLFW_ARROW_CURSOR,
            TYPING              = GLFW_IBEAM_CURSOR,
            CROSSHAIR           = GLFW_CROSSHAIR_CURSOR,
            HAND                = GLFW_HAND_CURSOR,
            HORIZONTAL_RESIZE   = GLFW_HRESIZE_CURSOR,
            VERTICAL_RESIZE     = GLFW_VRESIZE_CURSOR,
            __MAX               = GLFW_VRESIZE_CURSOR + 1
        };

    private:
        // Stores time last frame began, used for calculating elapsed time
        static Timer m_Timer;

        // Updates projection matrix, viewport, etc. when window dimensions are changed
        static void m_WindowResizeCallback(GLFWwindow* window, int nwidth, int nheight);

        static int m_FPS; // Frames per second
        static double m_TimePerFrame; // Time per frame in seconds

        static float m_Volume; // Volume of sounds in range 0.0 -> 1.0

        enum class STATS_INDEX : int {
            AVERAGE_TPF,
            FPS,
            PROCESSING_PERCENTAGE,
            PLAYER_POS
        };

        static constexpr double POLL_INTERVAL = 1.0; // Determines how often average performance is calculated (in seconds)
        static unsigned int m_FramesProcessed; // Tracks amount of frames that have been processed since the last poll
        static double m_ProcessingTime; // Tracks time spent processing since the last poll

        static std::unordered_map<STATS_INDEX, std::shared_ptr<Text>> m_StatsTextMap;

        static VersionNumber m_VersionNumber; // Application version number (basically unused)

        // Calculates average time per frame, and will display warning if engine is running behind
        static void m_CalculatePerformance(double dt);

        static std::unordered_map<CURSOR_TYPE, GLFWcursor*> m_CursorMap;

    public:
        static std::shared_ptr<TextureAtlas> engine_icons; // Texture for engine icons

        static GLFWwindow* window;  // Pointer to GLFW window
        static int width;           // Width of window
        static int height;          // Height of window

        static irrklang::ISoundEngine* sound_engine;

        static std::string resources_path; // Path to resources file

        static std::shared_ptr<Panel> window_panel;  // For attaching things to the window

        // Set clear screen/background color
        static void SetBGColor(const RGBColor& color);

        static void Init(int nwidth, int nheight, int nfps, const char* resources_path = "../Resources");
        static void Terminate();

        static void StartSound(const std::string& file);

        // Executes all pre-rendering operations
        static void BeginFrame();
        // Executes all post-rendering operations
        static void EndFrame();

        static void SetCursor(const CURSOR_TYPE& cursor);

        static void SetFPS(int new_fps);
        static void SetVolume(float volume);

        // TODO: pretty specific, only for games with players
        // TODO: should be moved to client/game side
        static void UpdateStats(const Body& player_body);

        static void EnableWireframe();
        static void DisableWireframe();

        static Vector2<int> GetScreenDim();

        // Returns if the engine should still be running
        static bool IsRunning();
    };
}