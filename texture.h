#include <array>
#include <map>

using std::array;
using std::map;

#define textureWidth 32
#define textureHeight 32

extern map<int, array<int, textureHeight * textureWidth>> textures;

void loadTextures();