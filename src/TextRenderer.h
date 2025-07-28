#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>

class TextRenderer
{
private:
    TTF_Font *font;
    SDL_Renderer *renderer;

public:
    TextRenderer(SDL_Renderer *renderer);
    ~TextRenderer();

    bool initialize(const std::string &fontPath, int fontSize);
    void renderText(const std::string &text, int x, int y, SDL_Color color = {255, 255, 255, 255});
    void renderCenteredText(const std::string &text, int x, int y, int width, int height, SDL_Color color = {255, 255, 255, 255});
};

#endif
