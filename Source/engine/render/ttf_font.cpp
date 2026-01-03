#include "engine/render/ttf_font.hpp"

#ifndef USE_SDL1

#include <SDL.h>
#ifdef USE_SDL3
#include <SDL3/SDL_ttf.h>
#else
#include <SDL_ttf.h>
#endif

#include "utils/log.hpp"
#include "utils/utf8.hpp"

namespace devilution {

void InitTTF()
{
	if (TTF_Init() == -1) {
		LogError("TTF_Init: {}", TTF_GetError());
	}
}

void QuitTTF()
{
	TTF_Quit();
}

TTFFontUniquePtr LoadTTFFont(int size, const char *path)
{
	TTF_Font *font = TTF_OpenFont(path, size);
	if (font == nullptr) {
		LogError("TTF_OpenFont: {}", TTF_GetError());
		return nullptr;
	}
	return TTFFontUniquePtr(font);
}

SDL_Surface *GlyphCache::GetGlyph(TTF_Font *font, char32_t codepoint, SDL_Color color)
{
	CacheKey key { codepoint, color };
	auto it = cache_.find(key);
	if (it != cache_.end()) {
		return it->second.get();
	}

	std::string text;
	AppendUtf8(codepoint, text);

	SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (surface == nullptr) {
		LogError("TTF_RenderUTF8_Blended: {}", TTF_GetError());
		return nullptr;
	}

	cache_[key] = SDLSurfaceUniquePtr(surface);
	return surface;
}

} // namespace devilution

#endif
