//
// Created by vegard on 16.01.2024.
//
#include "stringMath.hpp"
#include "external/exprtk.hpp"

class SymbolTableType: public exprtk::symbol_table<float>{};
class ParserType: public exprtk::parser<float>{};

std::string StringMath::calculate(const std::string &string){
    if(parser == nullptr){
        parser = std::make_shared<ParserType>();
    }
    if(symbolTable == nullptr){
        symbolTable = std::make_shared<SymbolTableType>();
    }
    auto result = parser->compile(string,*symbolTable).value();
    if(!isnan(result)){
        return std::to_string(int(result));
    }
    return string;
}