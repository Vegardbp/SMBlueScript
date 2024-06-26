//
// Created by vegard on 28.12.2023.
//

#ifndef BLUESCRIPT_BLUESEMBLY_HPP
#define BLUESCRIPT_BLUESEMBLY_HPP

#include "logic.hpp"
#include "codeWriter.hpp"
#include "stringFunctions.hpp"

class Bluesembly{
public:
    explicit Bluesembly(const std::shared_ptr<logic::LogicMaker> &logicMaker) : logicMaker(logicMaker) {}

    void generateGates(const std::vector<std::vector<std::string>> &content, const bool &debug){
        auto time = clock();
        CodeWriter writer("bluesembly.txt");
        int lineCount = 0;
        for(const std::vector<std::string>& line: content){
            writer.writeLine(line);
            if(debug) stringFunctions::print(line);
            read(line);
            lineCount++;
        }
        std::cout << "Ran " + std::to_string(lineCount) << " lines of Bluesembly in " << stringFunctions::secondsToTime((clock()-time)/1000.0) << std::endl;
    }

    bool isBluesembly(const std::vector<std::string> &line){
        for(auto &bluesemblyFunction: bluesemblyFunctions){
            if(line[0] == bluesemblyFunction){
                return true;
            }
        }
        return false;
    }

private:
    void read(const std::vector<std::string> &line){
        if(line[0] == bluesemblyFunctions[0]){
            generateLogic(line);
        }
        else if(line[0] == bluesemblyFunctions[1]){
            connect(line);
        }
        else if(line[0] == bluesemblyFunctions[2]){
            rename(line);
        }
        else if(line[0] == bluesemblyFunctions[3]){
            generateBlock(line);
        }
    }

    std::vector<std::string> bluesemblyFunctions = {
            "generateLogic",
            "connect",
            "rename",
            "generateBlock"
    };

    void generateLogic(const std::vector<std::string> &line){
        auto gate = logicMaker->generateLogicGate(line[1], logic::Mode::And);
        auto content = stringFunctions::getContent(line,1,"()");
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

    void generateBlock(const std::vector<std::string> &line){
        auto block = logicMaker->generateBlock();
        auto content = stringFunctions::getContent(line,1,"()");
        for(const std::vector<std::string> &args: content){
            if(args[0] == "position"){
                block->position = {std::stoi(args[1]), std::stoi(args[2]), std::stoi(args[3])};
            }
            if(args[0] == "bounds"){
                block->bounds = {std::stoi(args[2]), std::stoi(args[3]), std::stoi(args[1])};
            }
            else if(args[0] == "color"){
                block->color = args[1];
            }
            else if(args[0] == "shapeid"){
                block->shapeID = args[1];
            }
        }
    }

    void connect(const std::vector<std::string> &line){
        auto content = stringFunctions::getContent(line,1,"()");
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

    void rename(const std::vector<std::string> &line){
        auto content = stringFunctions::getContent(line,1,"()");
        logicMaker->rename(content[0], content[1][0]);
    }

    const std::shared_ptr<logic::LogicMaker> logicMaker;
};

#endif //BLUESCRIPT_BLUESEMBLY_HPP
