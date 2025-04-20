#pragma once

#include <string>

using namespace std;

/**
 * @brief Enumeration representing different data types.
 *
 * This enumeration represents different data types that can be used in the compiler.
 */
enum Type
{
    UNDEFINED, ///< Represents an undefined type.
    VOID,      ///< Represents the void type.
    INT,       ///< Represents the int type.
    CHAR,      ///< Represents the char type.
    DOUBLE     ///< Represents the double type.
};

/**
 * @brief A class responsible for managing data types.
 */
class TypeManager
{
public:
    /**
     * @brief Converts a string to the corresponding Type enum value.
     *
     * @param str The string representation of the type.
     * @return The corresponding Type enum value.
     */
    static Type stringToType(const string &str);

    /**
     * @brief Returns the size of a given type.
     *
     * @param t The type value.
     * @return The size of the type in bytes.
     */
    static int size_of(Type t);
};