#include <crow/Graphics.hpp>

#include "GraphicsOpenGL.hpp"

namespace crow {

    std::unique_ptr<Graphics> Graphics::CreateGraphics(const std::set<Graphics::GraphicsAPI>& apis) {
        return std::unique_ptr<Graphics>(new OpenGLGraphics);
    }

}