#ifndef CROW_HPP
#define CROW_HPP

#ifdef WINDOWS
#ifdef EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API
#endif

#endif