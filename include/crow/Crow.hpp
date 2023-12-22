#ifndef CROW_HPP
#define CROW_HPP

#ifdef WINDOWS
#ifdef EXPORT
/// @brief This is for Windows. This is needed to export/import DLL functions
#define API __declspec(dllexport)
#else
/// @brief This is for Windows. This is needed to export/import DLL functions
#define API __declspec(dllimport)
#endif
#else
/// @brief This is for Windows. This is needed to export/import DLL functions
#define API
#endif

#endif