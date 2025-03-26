#include <iostream>
#include <map>
#include <string>
#include "Type.h"
using namespace std;

class SymbolTable
{
protected:
    /**
     * @brief Construct a new Symbol Table object
     */
    SymbolTable()
    {
        this->parent = nullptr;
        this->children = vector<SymbolTable *>();
    }

    /**
     * @brief Construct a new Symbol Table object with a parent and an offset
     *
     * @param parent
     * @param currentOffset
     */
    SymbolTable(SymbolTable *parent, int currentOffset)
    {
        this->parent = parent;
        this->children = vector<SymbolTable *>();
        this->currentOffset = currentOffset;
    }

    /**
     * @brief add the symbol to the current symbol table and update the offset
     *
     * @param name The name of the symbol
     * @param type The type of the symbol
     */
    void addSymbol(string name, Type type);

    /**
     * @brief Get the Symbol Index object
     *
     * @param name The name of the symbol
     * @return int - The index of the symbol
     */
    int getSymbolIndex(string name);

    /**
     * @brief Returns true if the symbol is used
     *
     * @param name The name of the symbol
     * @return bool
     */
    bool symbolIsUsed(string name);

    /**
     * @brief Returns true if the symbol has a value
     *
     * @param name The name of the symbol
     * @return bool
     */
    bool symbolHasAValue(string name);

private:
    /**
     * @brief Returns true if the symbol table contains the symbol
     *
     * @param name The name of the symbol
     * @return bool
     */
    bool containsSymbol(string name);

    /// The symbols table containing variable names and their offsets.
    map<string, int> symbolsTable;

    /// The symbols table containing variable names and their types.
    map<string, Type> symbolsType;

    /// A map indicating if a variable has been used in the code.
    map<string, bool> symbolsUsage;

    /// A map to track whether a variable has been assigned a value.
    map<string, bool> symbolsHasAValue;

    SymbolTable *parent;
    vector<SymbolTable *> children;
    int currentOffset = -4;
};