#include "TypeManager.h"

Type TypeManager::stringToType(const string &str)
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

int TypeManager::size_of(Type t)
{
    if (t == Type::INT || t == Type::CHAR)
        return 4;
    if (t == Type::DOUBLE)
        return 16;
    return 1;
}