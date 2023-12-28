#include "codeReader.hpp"
#include "blueprintGenerator.hpp"

using namespace logic;

int main() {
    std::string path("C:/Users/vegar/AppData/Roaming/Axolot Games/Scrap Mechanic/User/User_76561198263032438/Blueprints/b69e41f1-e9f4-4197-8326-79c244896229");
    auto blueprintGenerator = BlueprintGenerator::create();
    auto logicMaker = LogicMaker::create();

    CodeReader reader(logicMaker);
    reader.fetch("code.txt");
    reader.read();

    blueprintGenerator->generate(path,logicMaker->gates);

    return 0;
}
