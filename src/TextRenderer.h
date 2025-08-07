#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <unordered_map>

class TextRenderer {
private:
    TTF_Font* font;
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture *> textCache;
    static const size_t MAX_CACHE_SIZE = 100;

public:
    TextRenderer();  // Changed: no parameters
    ~TextRenderer();
    
    bool initialize(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);  // Changed: added renderer parameter
    void renderText(const std::string& text, int x, int y, SDL_Color color = {255, 255, 255, 255});
    void renderCenteredText(const std::string& text, int x, int y, int width, int height, SDL_Color color);
    void renderCachedText(const std::string &text, int x, int y, SDL_Color color);
    void clearCache();
};

#endif
