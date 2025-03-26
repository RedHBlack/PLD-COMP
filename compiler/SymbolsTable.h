#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Type.h"

using namespace std;

/**
 * @brief This class represents a symbol table. It stores information about variables,
 * including their names, types, usage status, and values.
 */
class SymbolsTable
{
public:
    /**
     * @brief Construct a new Symbols Table object with an optional initial offset.
     *
     * @param currentOffset The initial offset (default is -4).
     */
    SymbolsTable(int currentOffset = -4) { this->currentOffset = currentOffset; }

    /**
     * @brief Add the symbol to the current symbol table and update the offset.
     *
     * @param name The name of the symbol.
     * @param type The type of the symbol.
     */
    void addSymbol(string name, Type type);

    /**
     * @brief Add a child symbol table to the current one.
     *
     * @param child The child symbol table to be added.
     */
    void addChild(SymbolsTable *child);

    /**
     * @brief Get the symbol index.
     *
     * @param name The name of the symbol.
     * @return int - The index of the symbol.
     */
    int getSymbolIndex(string name);

    /**
     * @brief Returns true if the symbol is used.
     *
     * @param name The name of the symbol.
     * @return bool - Whether the symbol has been used.
     */
    bool symbolIsUsed(string name);

    /**
     * @brief Returns true if the symbol has been assigned a value.
     *
     * @param name The name of the symbol.
     * @return bool - Whether the symbol has a value.
     */
    bool symbolHasAValue(string name);

    /**
     * @brief Sets the usage status of a symbol.
     *
     * @param name The name of the symbol.
     * @param isUsed Whether the symbol has been used.
     */
    void setSymbolUsage(string name, bool isUsed);

    /**
     * @brief Sets the definition status of a symbol (whether it has been assigned a value).
     *
     * @param name The name of the symbol.
     * @param hasValue Whether the symbol has a value assigned.
     */
    void setSymbolDefinitionStatus(string name, bool hasValue);

    /**
     * @brief Returns true if the symbol table contains the symbol.
     *
     * @param name The name of the symbol.
     * @return bool - Whether the symbol exists in the table.
     */
    bool containsSymbol(string name);

    /**
     * @brief Returns the symbols index mapping variable names to their offsets.
     *
     * @return map<string, int> - The symbols index.
     */
    map<string, int> getSymbolsIndex() { return symbolsIndex; }

    /**
     * @brief Returns the symbols type mapping variable names to their types.
     *
     * @return map<string, Type> - The symbols type.
     */
    map<string, Type> getSymbolsType() { return symbolsType; }

    /**
     * @brief Returns the symbols usage mapping variable names to their usage status.
     *
     * @return map<string, bool> - The symbols usage.
     */
    map<string, bool> getSymbolsUsage() { return symbolsUsage; }

    /**
     * @brief Returns the symbols definition status mapping variable names to their assignment status.
     *
     * @return map<string, bool> - The symbols definition status.
     */
    map<string, bool> getSymbolsDefinitionStatus() { return symbolsHasAValue; }

    /**
     * @brief Returns the child symbol tables.
     *
     * @return vector<SymbolsTable *> - The list of child symbol tables.
     */
    vector<SymbolsTable *> getChildren() { return children; }

    /**
     * @brief Returns the parent symbol table.
     *
     * @return SymbolsTable * - The parent symbol table.
     */
    SymbolsTable *getParent() { return parent; }

private:
    /// The symbols index containing variable names and their offsets.
    map<string, int> symbolsIndex;

    /// The symbols table containing variable names and their types.
    map<string, Type> symbolsType;

    /// A map indicating if a variable has been used in the code.
    map<string, bool> symbolsUsage;

    /// A map to track whether a variable has been assigned a value.
    map<string, bool> symbolsHasAValue;

    /// The parent symbol table.
    SymbolsTable *parent;

    /// The child symbol tables.
    vector<SymbolsTable *> children;

    /// The current offset for variables.
    int currentOffset;
};
