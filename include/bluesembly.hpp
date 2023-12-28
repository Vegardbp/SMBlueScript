//
// Created by vegard on 28.12.2023.
//

#ifndef BLUESCRIPT_BLUESEMBLY_HPP
#define BLUESCRIPT_BLUESEMBLY_HPP

#include "logic.hpp"
#include "codeWriter.hpp"

class Bluesembly{
public:
    explicit Bluesembly(const std::shared_ptr<logic::LogicMaker> &logicMaker) : logicMaker(logicMaker) {}

    void generateGates(const std::vector<std::vector<std::string>> &content){
        CodeWriter writer("bluesembly.txt");
        int lineCount = 0;
        for(const std::vector<std::string>& line: content){
            if(line.size() > 1){
                writer.writeLine(line);
                read(line);
                lineCount++;
            }
        }
        std::cout << "Ran " + std::to_string(lineCount) << " of Bluesembly." << std::endl;
    }

private:
    void read(const std::vector<std::string> &line){
        if(line[0] == "generateLogic" || line[0] == "gl"){
            auto gate = logicMaker->generateLogicGate(line[1], logic::Mode::And);
            auto content = getParenthesesContent(line);
            for(const std::vector<std::string> &args: content){
                if(args[0] == "position"){
                    gate->position = {std::stoi(args[1]), std::stoi(args[2]), std::stoi(args[3])};
                }
                else if(args[0] == "color"){
                    gate->color = args[1];
                }
                else if(args[0] == "mode"){
                    gate->mode = logic::modeFromString(args[1]);
                }
            }
        }
        else if(line[0] == "connect"){
            auto content = getParenthesesContent(line);
            for(const std::string &from: content[0]){
                for(const std::string &to: content[1]){
                    for(auto &fromName: logicMaker->gatesWithName(from)){
                        for(auto &toName: logicMaker->gatesWithName(to)){
                            fromName->connectTo(toName);
                        }
                    }
                }
            }
        }
        else if(line[0] == "rename"){
            auto content = getParenthesesContent(line);
            for(const std::string &from: content[0]){
                for(auto &gates: logicMaker->gatesWithName(from)){
                    gates->name = content[1][0];
                }
            }
        }
    }

    std::vector<std::vector<std::string>> getParenthesesContent(const std::vector<std::string> &line, const int start = 1){
        int insideParenthesesCount = 0;
        std::vector<std::vector<std::string>> allParentheses;
        std::vector<std::string> parenthesesContent;
        for(int i = start; i < line.size(); i++){
            if(stringContains(line[i], "(") > 0 && stringContains(line[i], ")") > 0){
                auto content = removeCharacter(line[i],"(");
                parenthesesContent.emplace_back(removeCharacter(content,")"));
                allParentheses.emplace_back(parenthesesContent);
                parenthesesContent.clear();
            }
            else if(stringContains(line[i], "(") > 0){
                if(insideParenthesesCount == 0){
                    parenthesesContent.emplace_back(removeCharacter(line[i],"("));
                }
                insideParenthesesCount += stringContains(line[i], "(");
            }
            else if(stringContains(line[i], ")") > 0){
                insideParenthesesCount -= stringContains(line[i], ")");
                if(insideParenthesesCount == 0){
                    parenthesesContent.emplace_back(removeCharacter(line[i],")"));
                    allParentheses.emplace_back(parenthesesContent);
                    parenthesesContent.clear();
                }
            }
            else if(insideParenthesesCount > 0){
                parenthesesContent.emplace_back(line[i]);
            }
        }
        return allParentheses;
    }

    int stringContains(const std::string &string, const std::string &contains){
        int count = 0;
        for(auto characters : string){
            if(characters == contains[0]){
                count++;
            }
        }
        return count;
    }

    std::string removeCharacter(const std::string &string, const std::string &character){
        std::string returnString;
        for(char characters : string){
            if(characters != character[0]){
                returnString += characters;
            }
        }
        return returnString;
    }

    const std::shared_ptr<logic::LogicMaker> logicMaker;
};

#endif //BLUESCRIPT_BLUESEMBLY_HPP
