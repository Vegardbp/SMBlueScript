//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_CODEREADER_HPP
#define BLUESCRIPT_CODEREADER_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "bluesembly.hpp"
#include "stringMath.hpp"

class Variable{
public:
    Variable(const std::string &name, const std::string &value) : name(name), value(value) {}

    const std::string name;
    std::string value;
};

class Function{
public:
    Function(const std::string &name, const std::vector<std::vector<std::string>> &function,
             const std::vector<std::string> &variableNames, const std::vector<std::string> &defaultValues) : name(name),
                                                                                                             function(
                                                                                                                     function),
                                                                                                             variableNames(
                                                                                                                     variableNames),
                                                                                                             defaultValues(
                                                                                                                     defaultValues) {}

    const std::string name;
    std::vector<std::vector<std::string>> function;
    std::vector<std::string> variableNames;
    std::vector<std::string> defaultValues;
};

class CodeReader {
public:
    CodeReader(const std::shared_ptr<logic::LogicMaker> &logicMaker) : logicMaker(logicMaker) {}

    void read(const std::string &fileName) {
        auto time = clock();
        content = fetch(fileName);
        bluesemblyCompiler = std::make_shared<Bluesembly>(logicMaker);
        int index = 0;
        while (index < content.size()) {
            auto line = content[index];
            compile(line);
            index++;
        }
        std::cout << "Read code and generated Bluesembly in " << stringFunctions::secondsToTime((clock() - time) / 1000.0) << std::endl;
        bluesemblyCompiler->generateGates(bluesembly,debugBluesembly);
    }

private:
    bool debugBluesembly = false;
    std::vector<std::vector<std::string>> fetch(const std::string &fileName) {
        std::vector<std::vector<std::string>> newContent;
        std::string text;
        std::ifstream codeFile(fileName);
        while (std::getline(codeFile, text)) {
            newContent.emplace_back(stringFunctions::splitWords(text));
        }
        codeFile.close();
        return newContent;
    }

    std::vector<std::string> performMath(const std::vector<std::string> &line) {
        std::vector<std::string> returnLine;
        for (std::string word: line) {
            bool isEquation = false;
            auto operators = {"-","+","/","*","^","sin","cos","tan","sqrt","log"};
            auto container = "()";
            if(!(word[0] == container[0] && word[word.size()-1] == container[1])){
                for(auto &ope: operators){
                    if(word.find(ope) != std::string::npos){
                        isEquation = true;
                        break;
                    }
                }
            }
            if(isEquation){
                returnLine.emplace_back(stringMath.calculate(word));
            }else{
                returnLine.emplace_back(word);
            }
        }
        return returnLine;
    }

    StringMath stringMath;
    bool debug = false;
    int bracketCount = 0;
    std::string bracket = "{}";
    std::vector<std::vector<std::string>> currentBracketContent;

    void compile(std::vector<std::string> line) {
        bracketCount += stringFunctions::contains(line, bracket[0]) - stringFunctions::contains(line, bracket[1]);
        if (bracketCount == 0) {
            line = mathAndVariables(line);
            if(debug){
                stringFunctions::print(line);
            }
            if (bluesemblyCompiler->isBluesembly(line)) {
                bluesembly.emplace_back(line);
            } else if (line[0] == "variable" || line[0] == "var") {
                generateVariable(line);
            } else if (line[0] == "debugBlue") {
                debugBluesembly = true;
            } else if (variableFromName(line[0]) != nullptr) {
                if (line[1] == "=") {
                    variableFromName(line[0])->value = line[2];
                }
            } else if (functionFromName(line[0]) != nullptr) {
                runFunction(functionFromName(line[0]), stringFunctions::getContent(line));
            } else if (line[0] == "import") {
                auto libraryContent = fetch(line[1] + ".txt");
                for (auto &libLine: libraryContent) {
                    compile(libLine);
                }
            } else if (line[0] == "debug") {
                debug = !debug;
            } else if (line[0] == "del" || line[0] == "delete") {
                removeVariable(line[1]);
            } else if (line[0] == "print") {
                stringFunctions::print(line);
            } else if (line[0] == "round") {
                roundVariable(line);
            } else if (line[0] == "int") {
                floorVariable(line);
            }
        } else {
            currentBracketContent.emplace_back(line);
        }
        if (bracketCount == 0) {
            if (!currentBracketContent.empty()) {
                compileBracketContent(currentBracketContent);
            }
        }
    }

    void compileBracketContent(std::vector<std::vector<std::string>> bracketContent) {
        currentBracketContent.clear();
        if (bracketContent[0][0] == "def" || bracketContent[0][0] == "define" || bracketContent[0][0] == "func" || bracketContent[0][0] == "function" || bracketContent[0][0] == "void" || bracketContent[0][0] == "void*") {
            generateDefinition(bracketContent);
        }else if (bracketContent[0][0] == "for") {
            runForLoop(bracketContent);
        }
        else if (bracketContent[0][0] == "if") {
            if (checkStringIfStatement(bracketContent)) {
                runBracketContent(bracketContent);
            }
        }
        else if (bracketContent[0][0] == "while") {
            while (checkStringIfStatement(bracketContent)) {
                runBracketContent(bracketContent);
            }
        }
    }

    std::vector<std::string> replaceVariables(const std::vector<std::string> &line) {
        std::vector<std::string> returnLine;
        for (auto &word: line) {
            std::string newWord = word;
            for (auto &variable: variables) {
                newWord = stringFunctions::replaceAll(newWord, "{" + variable->name + "}", variable->value);
            }
            returnLine.emplace_back(newWord);
        }
        return returnLine;
    }

    std::vector<std::string> mathAndVariables(std::vector<std::string> line) {
        for (int i = 0; i < 4; i++) {
            line = replaceVariables(line);
        }
        line = performMath(line);
        return line;
    }

    std::shared_ptr<Variable> variableFromName(const std::string &name){
        for(auto &variable: variables){
            if(variable->name == name){
                return variable;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Function> functionFromName(const std::string &name){
        for(auto &function: functions){
            if(function->name == name){
                return function;
            }
        }
        return nullptr;
    }

    void runFunction(const std::shared_ptr<Function> &function, const std::vector<std::vector<std::string>> &args){
        std::vector<std::optional<std::string>> originalVariables;
        auto varCount = function->variableNames.size();
        auto defaultCount = function->defaultValues.size();
        for(int i = 0; i < function->variableNames.size(); i++){
            auto var = variableFromName(function->variableNames[i]);
            if(var != nullptr){
                originalVariables.emplace_back(var->value);
            }else{
                originalVariables.emplace_back(std::nullopt);
            }
            if(i >= varCount-defaultCount){
                auto value = function->defaultValues[i-(varCount-defaultCount)];
                if(!value.empty()){
                    compile({"variable",function->variableNames[i], "=", value});
                }else{
                    compile({"variable",function->variableNames[i], "=", "0"});
                }
            }else{
                compile({"variable",function->variableNames[i], "=", "0"});
            }
        }
        for(int i = 0; i < args[0].size(); i++){
            if(!args[0][i].empty()){
                compile({function->variableNames[i],"=",args[0][i]});
            }
        }
        for(int i = 1; i < args.size(); i++){
            for(auto &variableName: function->variableNames){
                if(args[i][0] == variableName && args[i][1] == "="){
                    compile({"variable",variableName, "=", args[i][2]});
                }
            }
        }
        for(const auto &line: function->function){
            compile(line);
        }
        for(int i = 0; i < function->variableNames.size(); i++){
            if(originalVariables[i].has_value()){
                compile({function->variableNames[i], "=", originalVariables[i].value()});
            }else{
                compile({"delete", function->variableNames[i]});
            }
        }
    }

    void generateVariable(const std::vector<std::string> &line) {
        auto var = variableFromName(line[1]);
        if(var == nullptr){
            if(line.size() > 2){
                if(line[2] == "="){
                    variables.emplace_back(std::make_shared<Variable>(line[1],line[3]));
                }
            }else{
                variables.emplace_back(std::make_shared<Variable>(line[1],line[3]));
            }
        }else{
            if(line.size() > 2){
                var->value = line[3];
            }
        }
    }

    void roundVariable(const std::vector<std::string> &line) {
        auto var = variableFromName(line[1]);
        var->value = stringFunctions::to_string_with_precision(std::stod(var->value),std::stoi(line[2]));
    }

    void floorVariable(const std::vector<std::string> &line) {
        auto var = variableFromName(line[1]);
        var->value = std::to_string(std::stoi(var->value));
    }

    void removeVariable(const std::string &variableName) {
        std::vector<std::shared_ptr<Variable>> newVariableList;
        for(auto &variable: variables){
            if(variable->name != variableName){
                newVariableList.emplace_back(variable);
            }
        }
        variables = newVariableList;
    }

    void generateDefinition(const std::vector<std::vector<std::string>> &definitionContent){
        auto parenthesis = stringFunctions::getContent(definitionContent[0]);
        auto variableNames = parenthesis[0];
        std::vector<std::string> defaultValues;
        if(parenthesis.size() > 1){
            defaultValues = parenthesis[1];
        }
        std::vector<std::vector<std::string>> bracketContent;
        for(int i = 1; i < definitionContent.size(); i++){
            bracketContent.emplace_back(definitionContent[i]);
        }
        functions.emplace_back(std::make_shared<Function>(definitionContent[0][1], bracketContent, variableNames, defaultValues));
    }

    void runForLoop(const std::vector<std::vector<std::string>> &bracketContent){
        std::optional<std::string> originalVariable;
        auto var = variableFromName(bracketContent[0][1]);
        if(var != nullptr){
            originalVariable = var->value;
        }
        std::vector<std::vector<std::string>> loopContent;
        for(int i = 1; i < bracketContent.size()-1; i++){
            loopContent.emplace_back(bracketContent[i]);
        }
        auto args = stringFunctions::getContent(bracketContent[0])[0];
        args = mathAndVariables(args);
        if(args.size() == 1){
            args = {"0",args[0],"1"};
        }else if(args.size() == 2){
            args = {args[0],args[1],"1"};
        }else if(args.size() == 3){
            args = args;
        }
        for(float i = std::stof(args[0]); i < std::stof(args[1]); i += std::stof(args[2])){
            compile({"var", bracketContent[0][1], "=", std::to_string(i)});
            runBracketContent(bracketContent);
        }
        if(originalVariable.has_value()){
            compile({bracketContent[0][1], "=", originalVariable.value()});
        }else{
            compile({"delete", bracketContent[0][1]});
        }
    }

    void runBracketContent(const std::vector<std::vector<std::string>> &bracketContent){
        for(int i = 1; i < bracketContent.size(); i++){
            compile(bracketContent[i]);
        }
    }

    bool checkStringIfStatement(const std::vector<std::vector<std::string>> &bracketContent){
        auto statementLine = mathAndVariables(bracketContent[0]);
        if(debug){
            stringFunctions::print(statementLine);
        }
        if(statementLine[2] == "=="){
            if(statementLine[1] == statementLine[3]){
                return true;
            }
        }
        else if(statementLine[2] == "!="){
            if(statementLine[1] != statementLine[3]){
                return true;
            }
        }
        else if(statementLine[2] == ">"){
            if(std::stof(statementLine[1]) > std::stof(statementLine[3])){
                return true;
            }
        }
        else if(statementLine[2] == "<"){
            if(std::stof(statementLine[1]) < std::stof(statementLine[3])){
                return true;
            }
        }
        else if(statementLine[2] == ">="){
            if(std::stof(statementLine[1]) >= std::stof(statementLine[3])){
                return true;
            }
        }
        else if(statementLine[2] == "<="){
            if(std::stof(statementLine[1]) <= std::stof(statementLine[3])){
                return true;
            }
        }
        return false;
    }

    std::vector<std::shared_ptr<Variable>> variables;
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::vector<std::string>> bluesembly;
    std::vector<std::vector<std::string>> content;
    std::shared_ptr<logic::LogicMaker> logicMaker;
    std::shared_ptr<Bluesembly> bluesemblyCompiler;
};

#endif //BLUESCRIPT_CODEREADER_HPP
