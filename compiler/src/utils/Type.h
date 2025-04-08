#pragma once

#include <string>

/**
 * @brief Enumeration representing different data types.
 *
 * This enumeration represents different data types that can be used in the compiler.
 */
enum class Type
{
    UNDEFINED, ///< Represents an undefined type.
    VOID,      ///< Represents the void type.
    INT,       ///< Represents the int type.
    CHAR,      ///< Represents the char type.
    DOUBLE     ///< Represents the double type.
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

    return Type::UNDEFINED;
}

inline int size_of(Type t)
{
    if (t == Type::INT || t == Type::CHAR)
        return 4;
    if (t == Type::DOUBLE)
        return 16;
    return 1;
}