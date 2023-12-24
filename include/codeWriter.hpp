//
// Created by vegard on 24.12.2023.
//

#ifndef BLUESCRIPT_CODEWRITER_HPP
#define BLUESCRIPT_CODEWRITER_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>

class CodeWriter{
public:
    explicit CodeWriter(const std::string &fileName){
        file = std::make_shared<std::basic_ofstream<char, std::char_traits<char>>>(fileName);
    }
    ~CodeWriter(){
        file->close();
    }

    void writeLine(const std::string &line){
        writeLineHeapFix(*file,line);
        writeLineHeapFix(*file,"\n");
    }

    void writeLine(const std::vector<std::string> &line){
        for(auto &word: line){
            writeLineHeapFix(*file,word);
            writeLineHeapFix(*file," ");
        }
        writeLineHeapFix(*file,"\n");
    }

private:
    static void writeLineHeapFix(std::basic_ofstream<char, std::char_traits<char>> &file_, const std::string &line){
        file_ << line;
    }
    std::shared_ptr<std::basic_ofstream<char, std::char_traits<char>>> file;
};
#endif //BLUESCRIPT_CODEWRITER_HPP
