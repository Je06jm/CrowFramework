#ifndef CROW_WINDOW_HPP
#define CROW_WINDOW_HPP

#include <memory>
#include <string>
#include <tuple>

#include "Actor.hpp"

namespace crow {

    /// @brief This message indicates that the window should change resolution
    struct API WindowMessageSetResolution : public MessageBase {
        std::tuple<int, int> resolution;
    };

    /// @brief This message indicates that the window should/shouldn't be
    /// fullscreened
    struct API WindowMessageSetFullscreen : public MessageBase {
        bool fullscreen;
    };

    /// @brief This message indicates that the window should change it's title
    struct API WindowMessageSetTitle : public MessageBase {
        std::string title;
    };

    /// @brief Manages the OpenGL/Vulkan/DirectX window
    class API Window {
    public:
        /// @brief This does nothing, just a virtual deconstructor
        virtual ~Window() {}

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

        /// @brief Closes the window
        virtual void Close() = 0;

        /// @brief Returns if the window should close
        /// @return \c true if it should close, \c false otherwise
        virtual bool ShouldClose() const = 0;

        /// @brief Creates a new window for the selected rendering API
        /// @return A pointer to the new window
        static std::unique_ptr<Window> CreateWindow();
    };

}

#endif