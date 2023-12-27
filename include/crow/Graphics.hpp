#ifndef CROW_GRAPHICS_HPP
#define CROW_GRAPHICS_HPP

#include <set>
#include <memory>

#include "Crow.hpp"

namespace crow {
    
    class API Buffer {
    public:
        enum class Access {
            ReadOnly,
            WriteOnly,
            ReadWrite
        };

        enum class Usage {
            StaticCopy,
            StaticDraw,
            StaticRead,
            DynamicCopy,
            DynamicDraw,
            DynamicRead,
            StreamCopy,
            StreamDraw,
            StreamRead
        };

        virtual ~Buffer() = default;

        virtual void Create(Usage usage, size_t size) = 0;
        virtual void Destroy() = 0;

        virtual void Bind() = 0;

        virtual void* Map(Access access) = 0;

        template <typename T>
        inline T* MapAs(Access access) { return reinterpret_cast<T*>(Map(access)); }

        virtual void Unmap() = 0;
    };

    class API VertexBuffer : public Buffer {
    public:
        virtual ~VertexBuffer() = default;
    };

    class API ElementBuffer : public Buffer {
    public:
        virtual ~ElementBuffer() = default;
    };

    /*class API ShaderProgram {
    public:
        enum class Type {
            Vertex,
            Fragment,
            Compute
        };

        const Type type;
        ShaderProgram(Type type) : type{type} {}
        virtual ~ShaderProgram() = default;

    };*/

    class API Graphics {
    public:
        enum class GraphicsAPI {
            OpenGL
        };

        GraphicsAPI api;
        Graphics(GraphicsAPI api) : api{api} {}

        virtual ~Graphics() = default;

        virtual std::unique_ptr<VertexBuffer> CreateVertexBuffer() = 0;
        virtual std::unique_ptr<ElementBuffer> CreateElementBuffer() = 0;

        static std::unique_ptr<Graphics> CreateGraphics(const std::set<GraphicsAPI>& apis);
        inline static std::unique_ptr<Graphics> CreateGraphics() {
            return CreateGraphics({GraphicsAPI::OpenGL});
        }
    };

}

#endif