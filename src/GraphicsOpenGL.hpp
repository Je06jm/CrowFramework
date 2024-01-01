#ifndef CROW_GRAPHICS_OPEN_GL_HPP
#define CROW_GRAPHICS_OPEN_GL_HPP

#include <crow/Graphics.hpp>

namespace crow {

    class OpenGLGraphics : public Graphics {
    public:
        OpenGLGraphics() : Graphics(GraphicsAPI::OpenGL) {}
        
        std::unique_ptr<VertexBuffer> CreateVertexBuffer() override;
        std::unique_ptr<ElementBuffer> CreateElementBuffer() override;
        std::unique_ptr<UniformBuffer> CreateUniformBuffer() override;
        std::unique_ptr<ShaderStorageBuffer> CreateShaderStorageBuffer() override;
    };

}

#endif