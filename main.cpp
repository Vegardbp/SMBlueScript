#include "codeReader.hpp"
#include "blueprintGenerator.hpp"

using namespace logic;

int main() {
    std::string path("C:/Users/vegar/AppData/Roaming/Axolot Games/Scrap Mechanic/User/User_76561198263032438/Blueprints/b69e41f1-e9f4-4197-8326-79c244896229");
    auto blueprintGenerator = BlueprintGenerator::create();
    auto logicMaker = LogicMaker::create();

    auto time = clock();
    CodeReader reader(logicMaker);
    reader.read("code.txt");

    blueprintGenerator->generate(path,logicMaker->gates, logicMaker->blocks);

    std::cout << "Spent a total of " << stringFunctions::secondsToTime((clock()-time)/1000.0) << std::endl;

    return 0;
}
