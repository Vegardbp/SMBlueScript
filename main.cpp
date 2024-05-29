#include "codeReader.hpp"
#include "blueprintGenerator.hpp"

using namespace logic;

int main() {
    auto blueprintGenerator = BlueprintGenerator::create();
    auto logicMaker = LogicMaker::create();

    auto time = clock();
    CodeReader reader(logicMaker);
    reader.read("code.txt");

    blueprintGenerator->generate(logicMaker->gates, logicMaker->blocks);

    std::cout << "Spent a total of " << stringFunctions::secondsToTime((clock()-time)/1000.0) << std::endl;

    auto endTime = clock() + 100000;
    while(clock() < endTime){

    }

    return 0;
}
