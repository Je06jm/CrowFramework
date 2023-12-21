#ifndef CROW_TYPE_TRAITS_HPP
#define CROW_TYPE_TRAITS_HPP

#include <type_traits>

namespace crow {

    template <template <typename> class C, typename T>
    T _InternalInheritedDataType(const C<T>*);

    template <typename T>
    using InheritedDataType =
        decltype(_InternalInheritedDataType(std::declval<T*>()));

}

#endif