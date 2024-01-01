#include <crow/Window.hpp>

#include "GraphicsAPIs.hpp"

#include <cassert>
#include <iostream>

namespace crow {

    class CrossWindow : public Window {
    private:
        GLFWwindow* window = nullptr;

        int width = 1280;
        int height = 720;

        bool fullscreen = false;
        bool vsync = true;

        std::string title = "Crow Framework";

    public:
        CrossWindow(Graphics::GraphicsAPI api) : Window(api) { glfwInit(); }

        ~CrossWindow() {
            if (window != nullptr) { glfwDestroyWindow(window); }

            glfwTerminate();
        }

        void SetResolution(const std::tuple<int, int>& resolution) override {
            auto [x, y] = resolution;

            assert(x > 0 && "Invalid window width");
            assert(y > 0 && "Invalid window height");

            width = x;
            height = y;

            if (window) { glfwSetWindowSize(window, x, y); }
        }

        std::tuple<int, int> GetResolution() const override {
            return {width, height};
        }

        std::tuple<int, int> GetFullscreenResolution() const override {
            auto monitor = glfwGetPrimaryMonitor();
            auto mode = glfwGetVideoMode(monitor);

            return {mode->width, mode->height};
        }

        void SetFullscreen(bool is_fullscreen) override {
            fullscreen = is_fullscreen;

            if (window) {
                auto monitor = glfwGetPrimaryMonitor();
                auto mode = glfwGetVideoMode(monitor);

                glfwSetWindowMonitor(window, fullscreen ? monitor : nullptr, 0,
                                     0, width, height, mode->refreshRate);

                if (!fullscreen) Center();
            }
        }

        bool IsFullscreen() const override { return fullscreen; }

        void SetTitle(const std::string& title) override {
            this->title = title;

            if (window) { glfwSetWindowTitle(window, title.c_str()); }
        }

        std::string GetTitle() const { return title; }

        void SetVSync(bool enable_vsync) override {
            vsync = enable_vsync;

            if (window) {
                if (api == Graphics::GraphicsAPI::OpenGL) {
                    glfwSwapInterval(enable_vsync ? 1 : 0);
                }
            }
        }

        bool GetVSync() const override { return vsync; }

        void Center() override {
            if (!window) return;

            auto monitor = glfwGetPrimaryMonitor();
            auto mode = glfwGetVideoMode(monitor);

            int x = (mode->width / 2) - (width / 2);
            int y = (mode->height / 2) - (height / 2);

            glfwSetWindowPos(window, x, y);
        }

        void Update() override {
            if (!window) return;

            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        bool Create() override {
            if (api == Graphics::GraphicsAPI::OpenGL) {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
            }

            window = glfwCreateWindow(width, height, title.c_str(), nullptr,
                                      nullptr);

            if (!window) return false;

            Center();

            if (api == Graphics::GraphicsAPI::OpenGL) {
                glfwMakeContextCurrent(window);

                glfwSwapInterval(vsync ? 1 : 0);
            }

            int version = gladLoadGL(glfwGetProcAddress);

            if (version == 0) {
                glfwDestroyWindow(window);
                window = nullptr;

                return false;
            }

            if (api == Graphics::GraphicsAPI::OpenGL) {
#ifdef DEBUG
                int flags;
                glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                    glEnable(GL_DEBUG_OUTPUT);
                    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                    glDebugMessageCallback(
                        [](GLenum source, GLenum type, unsigned int id,
                           GLenum severity, GLsizei length, const char* message,
                           const void* params) {
                            std::cout << "OpenGL Error: " << message
                                      << std::endl;
                        },
                        nullptr);
                }
#endif
            }

            return true;
        }

        void Close() override {
            if (!window) return;

            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        bool ShouldClose() const override {
            if (!window) return false;

            return glfwWindowShouldClose(window);
        }
    };

    std::unique_ptr<Window> Window::CreateWindow(Graphics::GraphicsAPI api) {
        return std::unique_ptr<Window>(new CrossWindow(api));
    }

}