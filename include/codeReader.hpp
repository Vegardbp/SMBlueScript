//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_CODEREADER_HPP
#define BLUESCRIPT_CODEREADER_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "logic.hpp"
#include "blueprintGenerator.hpp"

class CodeReader {
public:
    CodeReader(const std::shared_ptr<logic::LogicMaker> &logicMaker,
               const std::shared_ptr<BlueprintGenerator> &blueprintGenerator) : logicMaker(logicMaker),
                                                                                blueprintGenerator(
                                                                                        blueprintGenerator) {}

    std::vector<std::vector<std::string>> fetch(const std::string &fileName) {
        content.clear();
        std::string myText;
        std::ifstream codeFile(fileName);
        while (std::getline(codeFile, myText)) {
            content.emplace_back(splitWords(myText));
        }
        codeFile.close();
        return content;
    }

    static std::vector<std::string> splitWords(const std::string &line) {
        std::vector<std::string> lineContent;
        std::string word;
        std::string space(" ");
        for (auto &letter: line) {
            if (letter != space[0]) {
                word += letter;
            } else {
                lineContent.emplace_back(word);
                word = "";
            }
        }
        lineContent.emplace_back(word);

        return lineContent;
    }

    static void printContent(const std::vector<std::string> &line) {
        if (line.empty()) {
            std::cout << "no content" << std::endl;
        }
        for (auto &words: line) {
            std::cout << words << " ";
        }
    }

    static void printContent(const std::vector<std::vector<std::string>> &text) {
        if (text.empty()) {
            std::cout << "no content" << std::endl;
        }
        for (auto &line: text) {
            printContent(line);
            std::cout << "\n";
        }
    }

private:
    std::vector<std::vector<std::string>> content;

    std::shared_ptr<logic::LogicMaker> logicMaker;
    std::shared_ptr<BlueprintGenerator> blueprintGenerator;
};

#endif //BLUESCRIPT_CODEREADER_HPP
