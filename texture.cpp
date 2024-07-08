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

map<int, array<int, 3 * textureHeight * textureWidth>> textures;

// Load textures from "textures" directoy. Textures must be named as "<texture_code>_<texture-name>" and are
// in a 3x32x32 format 3 is for RGB
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

        // Strip ppm metadata
        textureStr = textureStr.erase(0, textureStr.substr(0, textureStr.find_first_of("\n") + 1).length());
        textureStr = textureStr.erase(0, textureStr.substr(0, textureStr.find_first_of("\n") + 1).length());

        array<int, 3 * textureWidth * textureHeight> texture;
        int texturePos = 0;

        // Store texture pixel for parsing multi-digits
        string texturePixel = "";

        // Add characters to the texture array
        for (auto i = 0; i < textureStr.length(); i++) {
            if (textureStr[i] == ' ' || textureStr[i] == '\n' || textureStr[i] == ',') {
                if (texturePixel != "") {
                    texture[texturePos] = stoi(texturePixel);
                    texturePixel = "";
                    texturePos++;
                }
                continue;
            }

            if (texturePos >= 3 * textureWidth * textureHeight) {
                if (texturePixel != "") {
                    texture[texturePos] = stoi(texturePixel);
                    texturePixel = "";
                }
                break;
            }

            texturePixel += textureStr[i];
        }

        // Insert texture to array
        int textureCode = stoi(textureCodeStr);
        textures[textureCode] = texture;
    }
}