#include <crow/Window.hpp>

#include "GraphicsAPIs.hpp"

#include <cassert>

namespace crow {

    class CrossWindow : public Window {
    private:
        GLFWwindow* window = nullptr;

        int width = 1280;
        int height = 720;

        bool fullscreen = false;

        std::string title = "Crow Framework";

    public:
        CrossWindow() { glfwInit(); }

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
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(width, height, title.c_str(), nullptr,
                                      nullptr);

            if (!window) return false;

            Center();

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

    std::unique_ptr<Window> Window::CreateWindow() {
        return std::unique_ptr<Window>(new CrossWindow);
    }

}