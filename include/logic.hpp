//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_LOGIC_HPP
#define BLUESCRIPT_LOGIC_HPP

namespace logic{
    enum Mode{
        And,
        Or,
        Xor,
        Nand,
        Nor,
        Xnor
    };

    Mode modeFromString(const std::string &word){
        if(word == "or"){
            return Or;
        }else if(word == "xor"){
            return Xor;
        }else if(word == "nand"){
            return Nand;
        }else if(word == "nor"){
            return Nor;
        }else if(word == "xnor"){
            return Xnor;
        }
        return And;
    }

    class LogicGate{
    public:
        Mode mode = And;
        std::string color = "000000";
        std::vector<int> position = {0,0,0};
        std::vector<int> connections;
        int id = 0;
        std::string name;

        std::string generateLine(){
            std::string line;
            line += colorString + color;
            line += connectionString + generateConnectionString();
            line += idString + std::to_string(id);
            line += modeString + std::to_string(getMode());
            line += positionString + generatePositionString();
            line += shapeIdString;
            return line;
        }

        void connectTo(const std::shared_ptr<LogicGate> &other){
            connections.emplace_back(other->id);
        }

        int getMode(){
            switch (mode) {
                case Mode::And:
                    return 0;
                case Mode::Or:
                    return 1;
                case Mode::Xor:
                    return 2;
                case Mode::Nand:
                    return 3;
                case Mode::Nor:
                    return 4;
                case Mode::Xnor:
                    return 5;
            }
            return 0;
        }

    private:
        std::string generatePositionString(){
            std::string positionXString = R"({"x":)";
            std::string positionYString = R"(,"y":)";
            std::string positionZString = R"(,"z":)";
            return positionXString + std::to_string(position[0]) + positionYString + std::to_string(position[1]) + positionZString + std::to_string(position[2]) + "}";
        }

        std::string generateConnectionString(){
            std::string returnString;
            if(connections.empty()){
                returnString = "null";
            }else{
                returnString += "[";
                for(int i = 0; i < connections.size(); i++){
                    if(i != 0){
                        returnString += ",";
                    }
                    returnString += R"({"id":)" + std::to_string(connections[i]) + "}";
                }
                returnString += "]";
            }
            return returnString;
        }

        std::string colorString = R"({"color":")";
        std::string connectionString = R"(","controller":{"active":false,"controllers":)";
        std::string idString = R"(,"id":)";
        std::string modeString = R"(,"joints":null,"mode":)";
        std::string positionString = R"(},"pos":)";
        std::string shapeIdString = R"(,"shapeId":"9f0f56e8-2c31-4d83-996c-d00a9b296c3f","xaxis":-2,"zaxis":-1})";
    };

    class LogicMaker{
    public:
        std::vector<std::shared_ptr<LogicGate>> gates;

        std::shared_ptr<LogicGate> generateLogicGate(const std::string &name, const Mode &mode = And, const std::string &color = "000000",const std::vector<int> &position = {0,0,0}){
            auto gate = std::make_shared<LogicGate>();
            gate->id = gates.size()+6000;
            gate->mode = mode;
            gate->color = color;
            gate->position = position;
            gate->name = name;
            gates.emplace_back(gate);
            return gate;
        }

        static std::shared_ptr<LogicMaker> create(){
            return std::make_shared<LogicMaker>();
        }

        std::vector<std::shared_ptr<LogicGate>> gatesWithName(const std::string &name){
            std::vector<std::shared_ptr<LogicGate>> returnList;
            for(auto &gate: gates){
                if(gate->name == name){
                    returnList.emplace_back(gate);
                }
            }
            return returnList;
        };
    };
}

#endif //BLUESCRIPT_LOGIC_HPP
