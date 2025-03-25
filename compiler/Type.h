#pragma once

#include <string>

/**
 * @brief Enumeration representing different data types.
 */
enum class Type
{
    VOID,  ///< Represents the void type.
    INT,   ///< Represents the int type.
    CHAR,  ///< Represents the char type.
    DOUBLE ///< Represents the double type.
};

/**
 * @brief Converts a string to the corresponding Type enum value.
 *
 * @param str The string representation of the type.
 * @return The corresponding Type enum value.
 */
inline Type stringToType(const std::string &str)
{
    if (str == "void")
        return Type::VOID;
    if (str == "int")
        return Type::INT;
    if (str == "char")
        return Type::CHAR;
    if (str == "double")
        return Type::DOUBLE;
}
