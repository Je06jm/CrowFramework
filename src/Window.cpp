#include <crow/Window.hpp>

#include <crow/Logging.hpp>

#include "GraphicsAPIs.hpp"

#include <cassert>

namespace crow {

    class CrossWindow : public _InternalWindow {
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

        bool ShouldClose() const override {
            if (!window) return false;

            return glfwWindowShouldClose(window);
        }
    };

    std::unique_ptr<_InternalWindow> _InternalWindow::CreateWindow() {
        return std::unique_ptr<_InternalWindow>(new CrossWindow);
    }

    Window::Window() {
        window = _InternalWindow::CreateWindow();
    }

    void Window::HandleMessage(std::unique_ptr<WindowMessageBase>&& msg) {
        if (auto get_res = dynamic_cast<WindowGetResolution*>(msg.get())) {
            get_res->callback(window->GetResolution());
        }
        else if (auto get_full_res = dynamic_cast<WindowGetFullscreenResolution*>(msg.get())) {
            get_full_res->callback(window->GetFullscreenResolution());
        }
        else if (auto set_res = dynamic_cast<WindowSetResolution*>(msg.get())) {
            window->SetResolution(set_res->resolution);
        }
        else if (auto get_full = dynamic_cast<WindowGetFullscreen*>(msg.get())) {
            get_full->callback(window->IsFullscreen());
        }
        else if (auto set_full = dynamic_cast<WindowSetFullscreen*>(msg.get())) {
            window->SetFullscreen(set_full->fullscreen);
        }
        else if (auto get_title = dynamic_cast<WindowGetTitle*>(msg.get())) {
            get_title->callback(window->GetTitle());
        }
        else if (auto set_title = dynamic_cast<WindowSetTitle*>(msg.get())) {
            window->SetTitle(set_title->title);
        }
        else if (dynamic_cast<WindowCenter*>(msg.get())) {
            window->Center();
        }
        else if (dynamic_cast<WindowUpdate*>(msg.get())) {
            window->Update();
        }
        else if (dynamic_cast<WindowCreate*>(msg.get())) {
            window->Create();
        }
        else if (auto should_close = dynamic_cast<WindowShouldClose*>(msg.get())) {
            should_close->callback(window->ShouldClose());
        }
        else if (dynamic_cast<WindowClose*>(msg.get())) {
            window = nullptr;
        }
        else {
            engine::Error("Unhandled window message");
        }
    }

}