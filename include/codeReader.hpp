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
            line = replaceVariables(line);
            line = performMath(line);
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

    void compile(const std::vector<std::string> &line){
        if(bluesemblyCompiler->isBluesembly(line)){
            bluesembly.emplace_back(line);
        }else if(line[0] == "variable" || line[0] == "var"){
            generateVariable(line);
        }else if(variableFromName(line[0]) != nullptr){
            if(line[1] == "="){
                variableFromName(line[0])->value = line[2];
            }
        }else if(line[0] == "define" || line[0] == "def"){
            generateDefinition(line);
        }else if(functionFromName(line[0]) != nullptr){
            runFunction(functionFromName(line[0]), stringFunctions::getContent(line)[0]);
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
            auto newLine = line;
            newLine = replaceVariables(newLine);
            newLine = performMath(newLine);
            compile(newLine);
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

    void generateDefinition(const std::vector<std::string> &line){
        std::vector<std::vector<std::string>> bracketContent;
        const std::string container = "{}";
        auto definitionIndex = findLineIndex(line);
        int index = definitionIndex.value()+1;
        int bracketCount = 0;
        bracketCount += stringFunctions::contains(content[index], container[0]);
        while(bracketCount > 0){
            index += 1;
            bracketCount += stringFunctions::contains(content[index], container[0]);
            bracketCount -= stringFunctions::contains(content[index], container[1]);
            if(bracketCount > 0){
                bracketContent.emplace_back(content[index]);
            }
        }
        removeLine(definitionIndex.value(), index);
        auto variableNames = stringFunctions::getContent(line)[0];
        functions.emplace_back(std::make_shared<Function>(line[1],bracketContent,variableNames));
        for(auto &variableName: variableNames){
            compile({"variable",variableName, "=", "0"});
        }
    }

    std::optional<int> findLineIndex(const std::vector<std::string> &findLine){
        std::optional<int> index;
        for(int i = 0; i < content.size(); i++){
            if(content[i] == findLine){
                index = i;
                return index;
            }
        }
        return index;
    }

    void removeLine(const int &startIndex,const int &endIndex){
        std::vector<std::vector<std::string>> newContent;
        for(int i = 0; i < content.size(); i++){
            if(i < startIndex || i > endIndex){
                newContent.emplace_back(content[i]);
            }
        }
        content = newContent;
    }

    std::vector<std::shared_ptr<Variable>> variables;
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::vector<std::string>> bluesembly;
    std::vector<std::vector<std::string>> content;
    std::shared_ptr<logic::LogicMaker> logicMaker;
    std::shared_ptr<Bluesembly> bluesemblyCompiler;
};

#endif //BLUESCRIPT_CODEREADER_HPP
