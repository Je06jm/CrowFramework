#include <crow/Graphics.hpp>

#include "GraphicsAPIs.hpp"
#include "GraphicsOpenGL.hpp"

namespace crow {

    class OpenGLBuffer {
    protected:
        GLuint buffer = 0;
    
    public:
        void Create() {
            glGenBuffers(1, &buffer);
        }

        void Destroy() {
            glDeleteBuffers(1, &buffer);
        }

        static GLenum GetOpenGLUsage(Buffer::Usage usage) {
            switch (usage) {
                case Buffer::Usage::StaticCopy:
                    return GL_STATIC_COPY;
                
                case Buffer::Usage::StaticDraw:
                    return GL_STATIC_DRAW;
                
                case Buffer::Usage::StaticRead:
                    return GL_STATIC_READ;
                
                case Buffer::Usage::DynamicCopy:
                    return GL_DYNAMIC_COPY;
                
                case Buffer::Usage::DynamicDraw:
                    return GL_DYNAMIC_DRAW;
                
                case Buffer::Usage::DynamicRead:
                    return GL_DYNAMIC_READ;
                
                case Buffer::Usage::StreamCopy:
                    return GL_STREAM_COPY;
                
                case Buffer::Usage::StreamDraw:
                    return GL_STREAM_DRAW;
                
                default:
                    return GL_STREAM_READ;
            }
        }
    };

    class OpenGLVertexBuffer : public VertexBuffer, public OpenGLBuffer {
    public:
        void Create(Usage usage, size_t size) override {
            OpenGLBuffer::Create();
            Bind();
            
            GLenum gl_usage = GetOpenGLUsage(usage);
            glBufferData(GL_ARRAY_BUFFER, size, nullptr, gl_usage);
        }

        void Destroy() override { OpenGLBuffer::Destroy(); }

        void* Map(Access access) override {
            Bind();

            switch (access) {
                case Buffer::Access::ReadOnly:
                    return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
                
                case Buffer::Access::WriteOnly:
                    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                
                default:
                    return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            }
        }

        void Unmap() override {
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        
        void Bind() override {
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
        }
    };

    class OpenGLElementBuffer : public ElementBuffer, public OpenGLBuffer {
    public:
        void Create(Usage usage, size_t size) override {
            OpenGLBuffer::Create();
            Bind();

            GLenum gl_usage = GetOpenGLUsage(usage);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, gl_usage);
        }

        void Destroy() override { OpenGLBuffer::Destroy(); }
        
        void* Map(Access access) override {
            Bind();

            switch (access) {
                case Buffer::Access::ReadOnly:
                    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
                
                case Buffer::Access::WriteOnly:
                    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
                
                default:
                    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
            }
        }

        void Unmap() override {
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }
        
        void Bind() override {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        }
    };

    class OpenGLUniformBuffer : public UniformBuffer, public OpenGLBuffer {
    public:
        void Create(Usage usage, size_t size) override {
            OpenGLBuffer::Create();
            Bind();

            GLenum gl_usage = GetOpenGLUsage(usage);
            glBufferData(GL_UNIFORM_BUFFER, size, nullptr, gl_usage);
        }

        void Destroy() override { OpenGLBuffer::Destroy(); }

        void* Map(Access access) override {
            Bind();

            switch (access) {
                case Buffer::Access::ReadOnly:
                    return glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY);
                
                case Buffer::Access::WriteOnly:
                    return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
                
                default:
                    return glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
            }
        }

        void Unmap() override {
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }

        void Bind() override {
            glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        }
    };

    class OpenGLShaderStorageBuffer : public ShaderStorageBuffer, public OpenGLBuffer {
    public:
        void Create(Usage usage, size_t size) override {
            OpenGLBuffer::Create();
            Bind();

            GLenum gl_usage = GetOpenGLUsage(usage);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, gl_usage);
        }

        void Destroy() override { OpenGLBuffer::Destroy(); }

        void* Map(Access access) override {
            Bind();

            switch (access) {
                case Buffer::Access::ReadOnly:
                    return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
                
                case Buffer::Access::WriteOnly:
                    return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
                
                default:
                    return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
            }
        }

        void Unmap() override {
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        void Bind() override {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        }
    };

    std::unique_ptr<VertexBuffer> OpenGLGraphics::CreateVertexBuffer() {
        return std::unique_ptr<VertexBuffer>(new OpenGLVertexBuffer);
    }

    std::unique_ptr<ElementBuffer> OpenGLGraphics::CreateElementBuffer() {
        return std::unique_ptr<ElementBuffer>(new OpenGLElementBuffer);
    }

    std::unique_ptr<UniformBuffer> OpenGLGraphics::CreateUniformBuffer() {
        return std::unique_ptr<UniformBuffer>(new OpenGLUniformBuffer);
    }

    std::unique_ptr<ShaderStorageBuffer> OpenGLGraphics::CreateShaderStorageBuffer() {
        return std::unique_ptr<ShaderStorageBuffer>(new OpenGLShaderStorageBuffer);
    }

}