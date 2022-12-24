/*
  Utility templates
 */
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint>
#include <type_traits>
#include <vector>
#include <numeric>
#include <WString.h>

namespace gob
{

// Size of array.
template <class C> constexpr auto size(const C& c) -> decltype(c.size()) { return c.size(); }
template<typename T, size_t N> constexpr auto size(const T(&)[N]) noexcept -> size_t { return N; }

// Underlying
template <typename T> constexpr typename std::underlying_type<T>::type to_underlying(T e) noexcept
{
    return static_cast<typename std::underlying_type<T>::type>(e);
}

// Exists T.toString()?
template<class T> struct has_toString
{                                              
    template<class U, int d = (&U::toString, 0)> static std::true_type check(U*);
    static std::false_type check(...);
    static T* _cls;
    static const bool value = decltype(check(_cls))::value;
};

/* Like join function of Ruby. (For Integral, Floating-point, and String)
   join({1, 2, 3}, "/"); // "1/2/3"
 */
template<typename T, typename std::enable_if<std::is_floating_point<T>::value || std::is_integral<T>::value || std::is_same<T, String>::value, std::nullptr_t>::type = nullptr>
String join(const std::vector<T>& v, const char* separator = ", ")
{

    return !v.empty() ? std::accumulate(++v.cbegin(), v.cend(), String(v[0]),
                                        [&separator](const String& a, const T& b)
                                        {
                                            return a + separator + String(b);
                                        }): String();
}

// Like join function of Ruby. (For classes that has the toString function.)
template<typename T, typename std::enable_if<has_toString<T>::value, std::nullptr_t>::type = nullptr>
String join(const std::vector<T>& v, const char* separator = ", ")
{
    return !v.empty() ? std::accumulate(++v.cbegin(), v.cend(), v[0].toString(),
                                        [&separator](const String& a, const T& b)
                                        {
                                            return a + separator + b.toString();
                                        }) : String();
}

//
}
#endif
