#include "SymbolsTable.h"
#include <iostream>

using namespace std;

SymbolsTable::~SymbolsTable()
{
    for (SymbolsTable *child : this->children)
    {
        delete child;
    }
}

void SymbolsTable::addSymbol(string name, Type type, int symbolSize, int index)
{

    if (index != 0)
    {
        this->symbolsIndex[name] = index;
    }
    else
    {
        this->currentOffset -= symbolSize;
        this->symbolsIndex[name] = this->currentOffset;
    }

    this->symbolsType[name] = type;
    this->symbolsUsage[name] = false;
}

bool SymbolsTable::containsSymbol(string name)
{
    return this->symbolsIndex.find(name) != this->symbolsIndex.end();
}

void SymbolsTable::addChild(SymbolsTable *child)
{
    child->parent = this;
    this->children.push_back(child);
}

int SymbolsTable::getSymbolIndex(string name)
{
    if (this->containsSymbol(name))
    {
        return this->symbolsIndex[name];
    }

    if (this->parent)
    {
        return this->parent->getSymbolIndex(name);
    }

    return 0;
}

Type SymbolsTable::getSymbolType(string name)
{
    if (this->containsSymbol(name))
    {
        return this->symbolsType[name];
    }

    if (this->parent)
    {
        return this->parent->getSymbolType(name);
    }

    return Type::UNDEFINED;
}

bool SymbolsTable::symbolIsUsed(string name)
{
    if (this->containsSymbol(name))
    {
        return this->symbolsUsage[name];
    }
    if (this->parent)
    {
        return this->parent->symbolIsUsed(name);
    }
    return false;
}

bool SymbolsTable::symbolHasAValue(string name)
{
    if (this->containsSymbol(name))
    {
        return this->symbolsHasAValue[name];
    }

    if (this->parent)
    {
        return this->parent->symbolHasAValue(name);
    }

    return false;
}

void SymbolsTable::setSymbolUsage(string name, bool isUsed)
{
    this->symbolsUsage[name] = isUsed;
}

void SymbolsTable::setSymbolDefinitionStatus(string name, bool hasValue)
{
    this->symbolsHasAValue[name] = hasValue;
}
