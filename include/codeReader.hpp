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

class Variable{
public:
    Variable(const std::string &name, const std::string &value) : name(name), value(value) {}

    const std::string name;
    std::string value;
};

class Function{
public:
    Function(const std::string &name, const std::vector<std::vector<std::string>> &function,
             const std::vector<std::string> &variableNames) : name(name), function(function),
                                                              variableNames(variableNames) {}

    const std::string name;
    std::vector<std::vector<std::string>> function;
    std::vector<std::string> variableNames;
};

class CodeReader {
public:
    CodeReader(const std::shared_ptr<logic::LogicMaker> &logicMaker) : logicMaker(logicMaker) {}

    std::vector<std::vector<std::string>> fetch(const std::string &fileName) {
        content.clear();
        std::string myText;
        std::ifstream codeFile(fileName);
        while (std::getline(codeFile, myText)) {
            content.emplace_back(stringFunctions::splitWords(myText));
        }
        codeFile.close();
        return content;
    }

    void read() {
        bluesemblyCompiler = std::make_shared<Bluesembly>(logicMaker);
        int index  = 0;
        while(index < content.size()){
            auto line = content[index];
            compile(line);
            index++;
        }
        bluesemblyCompiler->generateGates(bluesembly);
    }

private:
    std::vector<std::string> performMath(const std::vector<std::string> &line){
        std::vector<std::string> returnLine;
        for(auto &word: line){
            returnLine.emplace_back(stringFunctions::stringMath(word));
        }
        return returnLine;
    }
    int bracketCount = 0;
    std::string bracket = "{}";
    std::vector<std::vector<std::string>> currentBracketContent;
    void compile(std::vector<std::string> line){
        line = replaceVariables(line);
        line = performMath(line);
        bracketCount += stringFunctions::contains(line,bracket[0]);
        if(bracketCount == 0){
            if(bluesemblyCompiler->isBluesembly(line)){
                bluesembly.emplace_back(line);
            }else if(line[0] == "variable" || line[0] == "var"){
                generateVariable(line);
            }else if(variableFromName(line[0]) != nullptr){
                if(line[1] == "="){
                    variableFromName(line[0])->value = line[2];
                }
            }else if(functionFromName(line[0]) != nullptr){
                runFunction(functionFromName(line[0]), stringFunctions::getContent(line)[0]);
            }
        }else{
            currentBracketContent.emplace_back(line);
        }
        bracketCount -= stringFunctions::contains(line,bracket[1]);
        if(bracketCount == 0){
            if(!currentBracketContent.empty()){
                compileBracketContent(currentBracketContent);
            }
        }
    }

    void compileBracketContent(std::vector<std::vector<std::string>> bracketContent){
        currentBracketContent.clear();
        if(bracketContent[0][0] == "def" || bracketContent[0][0] == "define"){
            generateDefinition(bracketContent);
        }
        if(bracketContent[0][0] == "for"){
            runForLoop(bracketContent);
        }
    }

    std::vector<std::string> replaceVariables(const std::vector<std::string> &line){
        std::vector<std::string> returnLine;
        for(auto &word: line){
            std::string newWord = word;
            for(auto &variable: variables){
                newWord = stringFunctions::replaceAll(newWord, "{" + variable->name + "}", variable->value);
            }
            returnLine.emplace_back(newWord);
        }
        return returnLine;
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

    void runFunction(const std::shared_ptr<Function> &function, const std::vector<std::string> &args){
        for(int i = 0; i < args.size(); i++){
            compile({function->variableNames[i],"=",args[i]});
        }
        for(const auto &line: function->function){
            compile(line);

        }
    }

    void generateVariable(const std::vector<std::string> &line) {
        if(line.size() > 2){
            if(line[2] == "="){
                variables.emplace_back(std::make_shared<Variable>(line[1],line[3]));
            }
        }else{
            variables.emplace_back(std::make_shared<Variable>(line[1],line[3]));
        }
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
        auto variableNames = stringFunctions::getContent(definitionContent[0])[0];
        std::vector<std::vector<std::string>> bracketContent;
        for(int i = 1; i < definitionContent.size()-1; i++){
            bracketContent.emplace_back(definitionContent[i]);
        }
        functions.emplace_back(std::make_shared<Function>(definitionContent[0][1], bracketContent, variableNames));
        for(auto &variableName: variableNames){
            compile({"variable",variableName, "=", "0"});
        }
    }

    void runForLoop(const std::vector<std::vector<std::string>> &bracketContent){
        std::vector<std::vector<std::string>> loopContent;
        for(int i = 1; i < bracketContent.size()-1; i++){
            loopContent.emplace_back(bracketContent[i]);
        }
        compile({"var", bracketContent[0][1], "=", "0"});
        auto temporaryLoopFunction = std::make_shared<Function>(Function("temporaryForFunctionInternal",loopContent,{bracketContent[0][1]}));
        auto args = stringFunctions::getContent(bracketContent[0])[0];
        if(args.size() == 1){
            args = {"0",args[0],"1"};
        }else if(args.size() == 2){
            args = {args[0],args[1],"1"};
        }else if(args.size() == 3){
            args = args;
        }
        for(float i = std::stof(args[0]); i < std::stof(args[1]); i += std::stof(args[2])){
            runFunction(temporaryLoopFunction,{std::to_string(i)});
        }
        removeVariable(bracketContent[0][1]);
    }

    std::vector<std::shared_ptr<Variable>> variables;
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::vector<std::string>> bluesembly;
    std::vector<std::vector<std::string>> content;
    std::shared_ptr<logic::LogicMaker> logicMaker;
    std::shared_ptr<Bluesembly> bluesemblyCompiler;
};

#endif //BLUESCRIPT_CODEREADER_HPP
