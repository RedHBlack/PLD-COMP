#include "IRInstrMove.h"
#include <iostream>
#include <cstring>

/**
 * @brief Constructor for the IRInstrMove instruction.
 */
IRInstrMove::IRInstrMove(BasicBlock *bb_, string src, string dest)
    : BaseIRInstr(bb_), src(src), dest(dest)
{
}

/**
 * @brief Generates assembly code for moving a value between registers and memory.
 */
void IRInstrMove::gen_asm(ostream &o)
{
    const int offsetSrcVar = this->getBB()->getCFG()->get_var_index(this->src);
    const int offsetDestinationVar = this->getBB()->getCFG()->get_var_index(this->dest);

    if (src[0] != '%' && dest[0] != '%')
    {
        o << "   movl " << offsetSrcVar << "(%rbp), %eax\n";
        o << "   movl %eax, " << offsetDestinationVar << "(%rbp)\n";
    }
    else if (dest[0] != '%')
    {
        o << "   movl " << src << ", " << offsetDestinationVar << "(%rbp)\n";
    }
    else if (src[0] != '%')
    {
        o << "   movl " << offsetSrcVar << "(%rbp), " << dest << "\n";
    }
    //Si c'est un tableau - Utile pour le return
    else if (strstr(dest.c_str(), "[") != NULL)
    {
        //On récupère le nom du tableau
        std::string tabName = dest.substr(0, dest.find("["));
        //On récupère l'index du tableau
        std::string index = dest.substr(dest.find("[") + 1, dest.find("]") - dest.find("[") - 1);
        //On récupère l'offset du tableau
        const int offsetTabVar = this->getBB()->getCFG()->get_var_index(tabName);
        
        //On calcule l'index de la case du tableau - 
        
        //Si c'est un const
        if (index[0] == '0' || index[0] == '1' || index[0] == '2' || index[0] == '3' || index[0] == '4' || index[0] == '5' || index[0] == '6' || index[0] == '7' || index[0] == '8' || index[0] == '9')
        {
            //On récupère l'index de la case du tableau
            const int offsetDestinationVar = offsetTabVar + std::stoi(index) * 4;
        }
        //si c'est une variable
        else
        {
            //On récupère l'index de la case du tableau
            

            const int offsetDestinationVar = offsetTabVar + this->getBB()->getCFG()->get_var_index(index) * 4;
        }

        const int offsetDestinationVar = offsetTabVar + std::stoi(index) * 4;

        //On récupère la valeur de la source
        const int offsetSrcVar = this->getBB()->getCFG()->get_var_index(this->src);

        o << "   movl " << offsetSrcVar << "(%rbp), %eax\n";
    }
    else
    {
        o << "   movl " << src << ", " << dest << "\n";
    }
}
