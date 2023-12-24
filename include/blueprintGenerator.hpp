//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_BLUEPRINTGENERATOR_HPP
#define BLUESCRIPT_BLUEPRINTGENERATOR_HPP

#include "codeWriter.hpp"
#include "logic.hpp"

class BlueprintGenerator{
public:
    void addGate(const std::shared_ptr<logic::LogicGate> &gate){
        logicGates.emplace_back(gate);
    }

    void generate(const std::string &path){
        int count = 0;
        CodeWriter writer(path + "/blueprint.json");
        writer.writeLine(startString);
        if(!logicGates.empty()){
            for(int i = 0; i < logicGates.size()-1; i++){
                writer.writeLine(logicGates[i]->generateLine() + ",");
                count++;
            }
            writer.writeLine(logicGates[logicGates.size()-1]->generateLine());
            count++;
        }
        writer.writeLine(endString);
        std::cout << "generated " + std::to_string(count) + " logic gates" << std::endl;
    }

    static std::shared_ptr<BlueprintGenerator> create(){
        return std::make_shared<BlueprintGenerator>();
    }

private:
    std::string startString = R"({"bodies":[{"childs":[)";
    std::string endString = R"(]}],"version":4})";
    std::vector<std::shared_ptr<logic::LogicGate>> logicGates;
};

#endif //BLUESCRIPT_BLUEPRINTGENERATOR_HPP
