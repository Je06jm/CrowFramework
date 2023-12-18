#ifndef CROW_ATTRIBUTE_HPP
#define CROW_ATTRIBUTE_HPP

#include <set>
#include <string>
#include <cstdint>

#define CROW_ATTRIBUTE(name) Attribute name = Attribute::CreateNew(#name)

namespace crow {

    struct Attribute {
        using ID = uint32_t;
        const ID id;

        const std::string name;

        inline friend bool operator==(const Attribute& lhs, const Attribute& rhs) {
            return lhs.id == rhs.id;
        }

        inline friend bool operator!=(const Attribute& lhs, const Attribute& rhs) {
            return lhs.id != rhs.id;
        }
    
    private:
        static ID next_free_id;

        Attribute(const std::string& name) : id{next_free_id++}, name{name} {}

    public:
        static inline Attribute CreateNew(const std::string& name) {
            return {name};
        }
    };

    class AttributeHolder {
    private:
        std::set<Attribute> attributes;
    
    protected:
        constexpr AttributeHolder() {}

        constexpr AttributeHolder(const std::set<Attribute>& attributes) {
            for (const auto& attribute : attributes) {
                this->attributes.insert(attribute);
            }
        }

        constexpr void AddAttribute(const Attribute& attribute) {
            attributes.insert(attribute);
        }

        inline constexpr bool HasAttribute(const Attribute& attribute) const {
            return attributes.find(attribute) != attributes.end();
        }
    };

}

namespace std {

    template <>
    struct hash<crow::Attribute> {
        inline std::size_t operator()(const crow::Attribute& attribute) const {
            return std::hash<uint32_t>{}(attribute.id);
        }
    };

}

#endif