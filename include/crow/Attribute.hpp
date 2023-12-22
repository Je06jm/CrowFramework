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

    /// @brief This defines an Attribute. Each Attribute have a unique ID and
    /// a name
    struct API Attribute {
        using ID = uint32_t;

    private:
        /// @brief A unique ID
        ID id;

        /// @brief The attribute name
        std::string name;

    public:
        /// @brief Gets the ID of the Attribute
        /// @return The ID of the Attribute
        inline ID GetID() const { return id; }

        /// @brief Gets the name of the Attribute
        /// @return The name of the Attribute
        inline std::string GetName() const { return name; }

        /// @brief Standard equality operator
        /// @param lhs Left hand side
        /// @param rhs Right hand side
        /// @return \c true if equal, \c false otherwise
        inline friend bool operator==(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id == rhs.id;
        }

        /// @brief Standard equality operator
        /// @param lhs Left hand side
        /// @param rhs Right hand side
        /// @return \c false if equal, \c true otherwise
        inline friend bool operator!=(const Attribute& lhs,
                                      const Attribute& rhs) {
            return lhs.id != rhs.id;
        }

        /// @brief Allows sorting of Attributes by ID
        /// @param lhs Left hand side
        /// @param rhs Right hand side
        /// @return \c true if lhs < rhs, \c false otherwise
        inline friend bool operator<(const Attribute& lhs,
                                     const Attribute& rhs) {
            return lhs.id < rhs.id;
        }

        /// @brief Allows sorting of Attributes by ID
        /// @param lhs Left hand side
        /// @param rhs Right hand side
        /// @return \c true if lhs > rhs, \c false otherwise
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
        /// @brief This provides a unique ID
        static ID next_free_id;

        /// @brief Sets the const variables
        /// @param id The Attribute ID
        /// @param name The Attribute name
        Attribute(ID id, const std::string& name) : id{id}, name{name} {}

    public:
        /// @brief Creates a new Attribute
        /// @param name The name of the new Attribute
        /// @return The new Attribute
        static inline Attribute CreateNew(const std::string& name) {
            return {next_free_id++, name};
        }
    };

    /// @brief This allows any class to hold any number of Attributes
    class API AttributeHolder {
    private:
        /// @brief The list of Attributes
        std::set<Attribute> attributes;

    protected:
        /// @brief This does nothing, just a constructor
        AttributeHolder() {}

        /// @brief This allows the creation of the derived class with a number
        /// of Attributes
        /// @param attributes The Attributes to create the derived class with
        AttributeHolder(const std::set<Attribute>& attributes) {
            for (const auto& attribute : attributes) {
                this->attributes.insert(attribute);
            }
        }

        /// @brief Adds an Attribute to the derived class
        /// @param attribute The Attribute
        void AddAttribute(const Attribute& attribute) {
            attributes.insert(attribute);
        }

    public:
        /// @brief Determins if the derived class has an Attribute
        /// @param attribute The Attribute to test
        /// @return \c true if the derived class has the Attribute, \c false
        /// otherwise
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