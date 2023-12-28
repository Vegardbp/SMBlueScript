//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_CODEREADER_HPP
#define BLUESCRIPT_CODEREADER_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "bluesembly.hpp"

class CodeReader {
public:
    CodeReader(const std::shared_ptr<logic::LogicMaker> &logicMaker) : logicMaker(logicMaker) {}

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

    void read() {
        Bluesembly bluesembly(logicMaker);
        bluesembly.generateGates(content);
    }

    static std::vector<std::string> splitWords(const std::string &line) {
        std::vector<std::string> lineContent;
        std::string word;
        std::string space(" ");
        for (auto &letter: line) {
            if (letter != space[0]) {
                word += letter;
            } else {
                if(!word.empty()){
                    lineContent.emplace_back(word);
                    word = "";
                }
            }
        }
        lineContent.emplace_back(word);
        return lineContent;
    }

private:
    std::vector<std::vector<std::string>> content;
    std::shared_ptr<logic::LogicMaker> logicMaker;
};

#endif //BLUESCRIPT_CODEREADER_HPP
