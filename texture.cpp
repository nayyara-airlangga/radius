#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "texture.h"

using std::ifstream;
using std::ostringstream;
using std::stoi;
using std::string;

namespace fs = std::filesystem;

const fs::path texturesDir{"textures"};

map<int, array<int, textureHeight * textureWidth>> textures;

// Load textures from "textures" directoy. Textures must be named as "<texture_code>_<texture-name>"
void loadTextures() {
    // Loop through all textures in textures directory
    for (auto const &textureFile : fs::directory_iterator(texturesDir)) {
        auto filePath = textureFile.path().string();

        // Get texture code
        auto fileName = textureFile.path().filename().string();
        auto textureCodeStr = fileName.substr(0, fileName.find_first_of('_'));

        ifstream fileStream(filePath);
        ostringstream out;

        // Read entire file to string
        out << fileStream.rdbuf();
        string textureStr = out.str();

        array<int, textureWidth * textureHeight> texture;
        int texturePos = 0;

        // Add characters to the texture array
        for (const auto ch : textureStr) {
            if (ch == ' ' || ch == '\n' || ch == ',') {
                continue;
            }

            if (texturePos >= textureWidth * textureHeight) {
                break;
            }

            texture[texturePos] = ch - '0';
            texturePos++;
        }

        // Insert texture to array
        int textureCode = stoi(textureCodeStr);
        textures[textureCode] = texture;
    }
}