#include <array>
#include <map>

using std::array;
using std::map;

#define textureWidth 32
#define textureHeight 32
#define skyWidth 120
#define skyHeight 80
#define skyTextureCode "99999" // Reserved texture code for sky

extern map<int, array<int, 3 * textureHeight * textureWidth>> textures;
extern array<int, 3 * skyHeight * skyWidth> skyTexture;

void loadTextures();