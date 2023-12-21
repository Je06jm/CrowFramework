#ifndef CROW_ATTRIBUTE_HPP
#define CROW_ATTRIBUTE_HPP

#include <cstdint>
#include <set>
#include <string>

#include "Crow.hpp"

#define CROW_ATTRIBUTE(name) extern const API Attribute name
#define CROW_DEFINE_ATTRIBUTE(name) \
    const Attribute name = Attribute::CreateNew(#name)

namespace crow {

    struct API Attribute {
        using ID = uint32_t;

    private:
        ID id;

        std::string name;

    public:
        inline ID GetID() const { return id; }

        inline std::string GetName() const { return name; }

        inline friend bool operator==(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id == rhs.id;
        }

        inline friend bool operator!=(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id != rhs.id;
        }

        inline friend bool operator<(const Attribute& lhs,
                                     const Attribute& rhs) {
            return lhs.id < rhs.id;
        }

        inline friend bool operator>(const Attribute& lhs,
                                     const Attribute& rhs) {
            return lhs.id > rhs.id;
        }

        inline friend bool operator<=(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id <= rhs.id;
        }

        inline friend bool operator>=(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id >= rhs.id;
        }

    private:
        static ID next_free_id;

        Attribute(ID id, const std::string& name) : id{id}, name{name} {}

    public:
        static inline Attribute CreateNew(const std::string& name) {
            return {next_free_id++, name};
        }
    };

    class API AttributeHolder {
    private:
        std::set<Attribute> attributes;

    protected:
        AttributeHolder() {}

        AttributeHolder(const std::set<Attribute>& attributes) {
            for (const auto& attribute : attributes) {
                this->attributes.insert(attribute);
            }
        }

        void AddAttribute(const Attribute& attribute) {
            attributes.insert(attribute);
        }

    public:
        inline bool HasAttribute(const Attribute& attribute) const {
            return attributes.find(attribute) != attributes.end();
        }
    };

}

namespace std {

    template <>
    struct API hash<crow::Attribute> {
        inline std::size_t operator()(const crow::Attribute& attribute) const {
            return std::hash<uint32_t>{}(attribute.GetID());
        }
    };

}

#endif