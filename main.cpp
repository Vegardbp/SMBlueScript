#include "codeReader.hpp"
#include "logic.hpp"
#include "blueprintGenerator.hpp"

using namespace logic;

int main() {
    auto blueprintGenerator = BlueprintGenerator::create();
    auto logicMaker = LogicMaker::create();

    CodeReader reader(logicMaker,blueprintGenerator);
    reader.fetch("codeInput.txt");

    blueprintGenerator->generate();

    return 0;
}
