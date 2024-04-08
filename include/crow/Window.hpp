#ifndef CROW_WINDOW_HPP
#define CROW_WINDOW_HPP

#include <memory>
#include <string>
#include <tuple>
#include <functional>

#include "Actor.hpp"

namespace crow {

    struct API WindowMessageBase {
        virtual ~WindowMessageBase() = default;
    };
    struct API WindowGetResolution : public WindowMessageBase {
        using Callback = std::function<void(const std::tuple<int, int>&)>;
        const Callback callback;

        WindowGetResolution(Callback callback) : callback{callback} {}
    };

    struct API WindowGetFullscreenResolution : public WindowMessageBase {
        using Callback = std::function<void(const std::tuple<int, int>&)>;
        const Callback callback;

        WindowGetFullscreenResolution(Callback callback) : callback{callback} {}
    };

    struct API WindowSetResolution : public WindowMessageBase {
        const std::tuple<int, int> resolution;

        WindowSetResolution(const std::tuple<int, int>& resolution) : resolution{resolution} {}
    };

    struct API WindowGetFullscreen : public WindowMessageBase {
        using Callback = std::function<void(bool)>;
        const Callback callback;

        WindowGetFullscreen(Callback callback) : callback{callback} {}
    };

    struct API WindowSetFullscreen : public WindowMessageBase {
        const bool fullscreen;

        WindowSetFullscreen(const bool fullscreen) : fullscreen{fullscreen} {}
    };

    struct API WindowGetTitle : public WindowMessageBase {
        using Callback = std::function<void(const std::string&)>;
        const Callback callback;

        WindowGetTitle(Callback callback) : callback{callback} {}
    };

    struct API WindowSetTitle : public WindowMessageBase {
        const std::string title;

        WindowSetTitle(const std::string& title) : title{title} {}
    };

    struct API WindowCenter : public WindowMessageBase {};

    struct API WindowUpdate : public WindowMessageBase {};

    struct API WindowCreate : public WindowMessageBase {};

    struct API WindowShouldClose : public WindowMessageBase {
        using Callback = std::function<void(bool)>;
        const Callback callback;

        WindowShouldClose(Callback callback) : callback{callback} {}
    };

    struct API WindowClose : public WindowMessageBase {};

    /// @brief Manages the OpenGL/Vulkan/DirectX window
    class API _InternalWindow {
    public:
        /// @brief This does nothing, just a virtual deconstructor
        virtual ~_InternalWindow() {}

        /// @brief Sets the window's resolution
        /// @param resolution The resolution <width, height> of the window
        virtual void SetResolution(const std::tuple<int, int>& resolution) = 0;

        /// @brief Return the window's resolution
        /// @return The resolution <width, height>
        virtual std::tuple<int, int> GetResolution() const = 0;

        /// @brief Return the screen's current resolution
        /// @return The resolution <width, height>
        virtual std::tuple<int, int> GetFullscreenResolution() const = 0;

        /// @brief Sets the window's fullscreen status
        /// @param is_fullscreen \c true if for fullscreen, \c false for
        /// windowed
        virtual void SetFullscreen(bool is_fullscreen) = 0;

        /// @brief Return the window's fullscreen status
        /// @return \c true if it's fullscreened, \c false otherwise
        virtual bool IsFullscreen() const = 0;

        /// @brief Sets the window's title
        /// @param title The title
        virtual void SetTitle(const std::string& title) = 0;

        /// @brief Returns the window's title
        /// @return The title
        virtual std::string GetTitle() const = 0;

        /// @brief Centers the window if it's not fullscreened
        virtual void Center() = 0;

        /// @brief Updates the window. This needs to be called at the beginning
        /// of each frame
        virtual void Update() = 0;

        /// @brief This creates the window
        /// @return \c true if the window was created, \c false otherwise
        virtual bool Create() = 0;

        /// @brief Returns if the window should close
        /// @return \c true if it should close, \c false otherwise
        virtual bool ShouldClose() const = 0;

        /// @brief Creates a new window for the selected rendering API
        /// @return A pointer to the new window
        static std::unique_ptr<_InternalWindow> CreateWindow();
    };

    class API Window : public Actor<WindowMessageBase> {
    private:
        std::unique_ptr<_InternalWindow> window = nullptr;
    
    public:
        Window();
        ~Window() = default;

        void HandleMessage(std::unique_ptr<WindowMessageBase>&& msg) override;

        bool MainThreadOnly() const override { return true; }
    };

}

#endif