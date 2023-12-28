//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_BLUEPRINTGENERATOR_HPP
#define BLUESCRIPT_BLUEPRINTGENERATOR_HPP

#include "codeWriter.hpp"
#include "logic.hpp"

class BlueprintGenerator{
public:
    void generate(const std::string &path, const std::vector<std::shared_ptr<logic::LogicGate>> &logicGates){
        int connectionCount = 0;
        CodeWriter writer(path + "/blueprint.json");
        writer.writeLine(startString);
        if(!logicGates.empty()){
            for(int i = 0; i < logicGates.size()-1; i++){
                connectionCount += logicGates[i]->connections.size();
                writer.writeLine(logicGates[i]->generateLine() + ",");
            }
            writer.writeLine(logicGates[logicGates.size()-1]->generateLine());
            connectionCount += logicGates[logicGates.size()-1]->connections.size();
        }
        writer.writeLine(endString);
        std::cout << "Generated " + std::to_string(logicGates.size()) + " logic gates, and " + std::to_string(connectionCount) + " connections." << std::endl;
    }

    static std::shared_ptr<BlueprintGenerator> create(){
        return std::make_shared<BlueprintGenerator>();
    }

private:
    std::string startString = R"({"bodies":[{"childs":[)";
    std::string endString = R"(]}],"version":4})";
};

#endif //BLUESCRIPT_BLUEPRINTGENERATOR_HPP
