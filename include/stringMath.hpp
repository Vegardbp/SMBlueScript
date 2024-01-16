//
// Created by vegard on 16.01.2024.
//

#ifndef BLUESCRIPT_STRINGMATH_HPP
#define BLUESCRIPT_STRINGMATH_HPP

#include <string>
#include <memory>

class ParserType;
class SymbolTableType;

class StringMath{
public:
    std::string calculate(const std::string &string);
private:
    std::shared_ptr<SymbolTableType> symbolTable;
    std::shared_ptr<ParserType> parser;
};

#endif //BLUESCRIPT_STRINGMATH_HPP
