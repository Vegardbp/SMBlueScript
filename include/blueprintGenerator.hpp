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

    void generate(){
        CodeWriter writer("C:/Users/vegar/AppData/Roaming/Axolot Games/Scrap Mechanic/User/User_76561198263032438/Blueprints/b69e41f1-e9f4-4197-8326-79c244896229/blueprint.json");
        writer.writeLine(startString);
        for(int i = 0; i < logicGates.size()-1; i++){
            writer.writeLine(logicGates[i]->generateLine() + ",");
        }
        writer.writeLine(logicGates[logicGates.size()-1]->generateLine());
        writer.writeLine(endString);
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
