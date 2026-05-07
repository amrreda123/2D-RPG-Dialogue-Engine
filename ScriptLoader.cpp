#include "include/ScriptLoader.h"
#include "include/Globals.h"
#include "include/json.hpp" // مكتبة الـ JSON
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void ScriptLoader::loadScript(const std::string &filename) {
    script.clear();
    currentLine = 0;
    std::ifstream file(filename);
    
    if (file.is_open()) {
        json j;
        file >> j;
        for (auto &item : j) {
            DialogueLine line;
            line.speaker = item.value("speaker", "");
            line.text = item.value("text", "");
            
            if (item.contains("color")) {
                line.r = item["color"][0];
                line.g = item["color"][1];
                line.b = item["color"][2];
            } else {
                line.r = 1.0f;
                line.g = 1.0f;
                line.b = 1.0f;
            }
            
            line.isChoice = item.value("is_choice", false);
            if (line.isChoice) {
                line.opt1 = item.value("opt1", "");
                line.file1 = item.value("file1", "");
                line.opt2 = item.value("opt2", "");
                line.file2 = item.value("file2", "");
            }
            script.push_back(line);
        }
        std::cout << "SUCCESS: Script loaded from " << filename << std::endl;
    } else {
        std::cout << "ERROR: Failed to load script from " << filename << std::endl;
    }
}