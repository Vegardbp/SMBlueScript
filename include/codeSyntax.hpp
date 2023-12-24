//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_CODESYNTAX_HPP
#define BLUESCRIPT_CODESYNTAX_HPP

#include "logic.hpp"
#include "blueprintGenerator.hpp"

class Syntax {
public:
    void readLine(const std::vector<std::string> &line) {
        if(!line.empty()){
            read(line);
        }
    }

    static std::shared_ptr<Syntax> create(const std::shared_ptr<logic::LogicMaker> &logicMaker, const std::shared_ptr<BlueprintGenerator> &blueprintGenerator){
        auto syntax = std::make_shared<Syntax>();
        syntax->logicMaker = logicMaker;
        syntax->blueprintGenerator = blueprintGenerator;
        return syntax;
    }

private:
    std::shared_ptr<logic::LogicMaker> logicMaker;
    std::shared_ptr<BlueprintGenerator> blueprintGenerator;
    std::shared_ptr<logic::LogicGate> lastIf;

    static std::vector<std::string> removeWord(const std::vector<std::string> &line, const int &index){
        std::vector<std::string> returnLine;
        for(int i = 0; i < line.size(); i++){
            if(i != index){
                returnLine.emplace_back(line[i]);
            }
        }
        return returnLine;
    }

    static std::string removeCharacter(const std::string &word, const int &index){
        std::string returnWord;
        for(int i = 0; i < word.size(); i++){
            if(i != index){
                returnWord += word[i];
            }
        }
        return returnWord;
    }

    void read(const std::vector<std::string> &line) {
        if(line[0] == "if"){
            auto ifStatement = getParenthesesContent(line);
            if(ifStatement.size() == 1){
                lastIf = logicMaker->gatesWithName(ifStatement[0])[0];
            }
        }
        else if(line[0] == "generateLogic" || line[0] == "gl"){
            generateLogic(line);
        }
        else if(!logicMaker->gatesWithName(line[0]).empty() && lastIf != nullptr){
            lastIf->connectTo(logicMaker->gatesWithName(line[0])[0]);
        }
        else if(line[0] == "}"){
            lastIf = nullptr;
        }
    }

    static std::vector<std::string> getParenthesesContent(const std::vector<std::string> &line) {
        std::vector<std::string> returnLine;
        bool inParentheses = false;
        std::string parentheses = "()";
        for(auto &word: line){
            std::string word_ = word;
            if(word_[0] == parentheses[0]){
                inParentheses = true;
                word_ = removeCharacter(word_,0);
            }
            if(word_[word_.size()-1] == parentheses[1]){
                inParentheses = false;
                word_ = removeCharacter(word_,word_.size()-1);
                returnLine.emplace_back(word_);
            }
            else if(inParentheses){
                returnLine.emplace_back(word_);
            }
        }
        return returnLine;
    }

    void generateLogic(const std::vector<std::string> &line){
        auto posString = getParenthesesContent(line);
        std::vector<int> position = {std::stoi(posString[0]),std::stoi(posString[1]),std::stoi(posString[2])};
        auto mode = logic::modeFromString(line[6]);
        auto gate = logicMaker->generateLogicGate(line[1], mode, line[5], position);
        blueprintGenerator->addGate(gate);
    }

    static void printLine(const std::vector<std::string> &line){
        for(auto &word: line){
            std::cout << word << " ";
        }
        std::cout << "\n";
    }
};

#endif //BLUESCRIPT_CODESYNTAX_HPP
