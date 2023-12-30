//
// Created by vegard on 28.12.2023.
//

#ifndef BLUESCRIPT_STRINGFUNCTIONS_HPP
#define BLUESCRIPT_STRINGFUNCTIONS_HPP

#include <string>
#include <vector>

namespace stringFunctions{
    void printLine(const std::vector<std::string> &line){
        for(auto &word: line){
            std::cout << word << " ";
        }
        std::cout << std::endl;
    }

    std::string stringMath(const std::string &string){

        exprtk::symbol_table<float> symbolTable;

        exprtk::parser<float> parser;
        auto result = parser.compile(string,symbolTable).value();
        if(!isnan(result)){
            return std::to_string(int(result));
        }
        return string;
    }

    std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    } //from https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string

    std::vector<std::string> splitWords(const std::string &line) {
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

    int contains(const std::string &string, const char &contains_){
        int count = 0;
        for(auto characters : string){
            if(characters == contains_){
                count++;
            }
        }
        return count;
    }

    int contains(const std::vector<std::string> &line, const char &contains_){
        int count = 0;
        for(auto &word: line){
            count += contains(word,contains_);
        }
        return count;
    }

    std::string removeCharacter(const std::string &string, const char &character){
        std::string returnString;
        for(char characters : string){
            if(characters != character){
                returnString += characters;
            }
        }
        return returnString;
    }
    
    std::vector<std::vector<std::string>> getContent(const std::vector<std::string> &line, const int start = 1, const std::string &container = "()"){
        int insideParenthesesCount = 0;
        std::vector<std::vector<std::string>> allParentheses;
        std::vector<std::string> parenthesesContent;
        for(int i = start; i < line.size(); i++){
            if(contains(line[i], container[0]) > 0 && contains(line[i], container[1]) > 0){
                auto content = removeCharacter(line[i],container[0]);
                parenthesesContent.emplace_back(removeCharacter(content,container[1]));
                allParentheses.emplace_back(parenthesesContent);
                parenthesesContent.clear();
            }
            else if(contains(line[i], container[0]) > 0){
                if(insideParenthesesCount == 0){
                    parenthesesContent.emplace_back(removeCharacter(line[i],container[0]));
                }
                insideParenthesesCount += contains(line[i], container[0]);
            }
            else if(contains(line[i], container[1]) > 0){
                insideParenthesesCount -= contains(line[i], container[1]);
                if(insideParenthesesCount == 0){
                    parenthesesContent.emplace_back(removeCharacter(line[i],container[1]));
                    allParentheses.emplace_back(parenthesesContent);
                    parenthesesContent.clear();
                }
            }
            else if(insideParenthesesCount > 0){
                parenthesesContent.emplace_back(line[i]);
            }
        }
        return allParentheses;
    }
};

#endif //BLUESCRIPT_STRINGFUNCTIONS_HPP
