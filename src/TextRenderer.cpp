#include "TextRenderer.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <vector>

TextRenderer::TextRenderer(SDL_Renderer *renderer) : font(nullptr), renderer(renderer) {}

TextRenderer::~TextRenderer()
{
    if (font)
    {
        TTF_CloseFont(font);
    }
}

// bool TextRenderer::initialize(const std::string &fontPath, int fontSize)
// {
//     if (TTF_Init() == -1)
//     {
//         std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
//         return false;
//     }

//     font = TTF_OpenFont(fontPath.c_str(), fontSize);
//     if (!font)
//     {
//         // Try to use a system font if the specified font doesn't exist
//         font = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Regular.ttf", fontSize);
//         if (!font)
//         {
//             std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
//             return false;
//         }
//     }

//     return true;
// }

bool TextRenderer::initialize(const std::string &fontPath, int fontSize)
{
    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    std::vector<std::string> fontPaths = {
        fontPath, 
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/google-noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Arial.ttf",                // macOS 
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" // Ubuntu
    };

    for (const auto &path : fontPaths)
    {
        if (path.empty())
            continue;

        font = TTF_OpenFont(path.c_str(), fontSize);
        if (font)
        {
            std::cout << "Successfully loaded font: " << path << std::endl;
            return true;
        }
    }

    std::cerr << "Could not load any font. TTF_GetError: " << TTF_GetError() << std::endl;
    return false;
}

void TextRenderer::renderText(const std::string &text, int x, int y, SDL_Color color)
{
    if (!font)
        return;

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface)
        return;

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture)
        return;

    int width, height;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &width, &height);

    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);

    SDL_DestroyTexture(textTexture);
}

void TextRenderer::renderCenteredText(const std::string &text, int x, int y, int width, int height, SDL_Color color)
{
    if (!font)
        return;

    int textWidth, textHeight;
    TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

    int centeredX = x + (width - textWidth) / 2;
    int centeredY = y + (height - textHeight) / 2;

    renderText(text, centeredX, centeredY, color);
}
