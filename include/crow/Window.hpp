#ifndef CROW_WINDOW_HPP
#define CROW_WINDOW_HPP

#include <memory>
#include <string>
#include <tuple>

#include "Actor.hpp"

namespace crow {

    struct API WindowMessageSetResolution : public MessageBase {
        std::tuple<int, int> resolution;
    };

    struct API WindowMessageSetFullscreen : public MessageBase {
        bool fullscreen;
    };

    struct API WindowMessageSetTitle : public MessageBase {
        std::string title;
    };

    class API Window {
    public:
        virtual ~Window() {}

        virtual void SetResolution(const std::tuple<int, int>& resolution) = 0;
        virtual std::tuple<int, int> GetResolution() const = 0;

        virtual std::tuple<int, int> GetFullscreenResolution() const = 0;
        virtual void SetFullscreen(bool is_fullscreen) = 0;
        virtual bool IsFullscreen() const = 0;

        virtual void SetTitle(const std::string& title) = 0;
        virtual std::string GetTitle() const = 0;

        virtual void Center() = 0;

        virtual void Update() = 0;

        virtual bool Create() = 0;

        virtual void Close() = 0;

        virtual bool ShouldClose() const = 0;

        static std::unique_ptr<Window> CreateWindow();
    };

}

#endif