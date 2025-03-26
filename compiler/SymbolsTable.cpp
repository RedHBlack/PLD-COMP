#include "SymbolsTable.h"
#include <iostream>
using namespace std;

void SymbolTable::addSymbol(string name, Type type)
{
    this->symbolsIndex[name] = this->currentOffset;
    this->symbolstype[name] = type;
    this->currentOffset -= 4; // Déplacer l’offset mémoire
}

int SymbolTable::getSymbolIndex(string name)
{
    if (this->containsSymbol(name))
    {
        return this->symbolsIndex[name];
    }
    else
    {
        // remonter dans le parent
        if (this->parent != nullptr)
        {
            return this->parent->getSymbolIndex(name);
        }
        else
        {
            // Cas du main qui n'a pas de parent
            cerr << "Error: Variable '" << name << "' not declared.\n";
            exit(1);
        }
    }
}

bool SymbolTable::symbolIsUsed(string name)
{
    if (this->containsSymbol(name))
    {
        return this->isUsed[name];
    }
    else
    {
        if (this->parent != nullptr)
        {
            return this->parent->symbolIsUsed(name);
        }
        else
        {
            cerr << "#WARNING : Variable '" << name << " not used.\n";
        }
    }
}

bool SymbolTable::symbolHasAValue(string name)
{
    if (this->containsSymbol(name))
    {
        return this->hasAValue[name];
    }
    else
    {
        // remonter dans le parent
        if (this->parent != nullptr)
        {
            return this->parent->symbolHasAValue(name);
        }
        else
        {
            cerr << "#WARNING : Variable '" << name << " not initialized.\n";
        }
    }
}

bool SymbolTable::containsSymbol(string name)
{
    return this->symbolsIndex.find(name) != this->symbolsIndex.end();
}