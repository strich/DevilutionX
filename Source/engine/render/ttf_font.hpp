#pragma once

#ifndef USE_SDL1

#include <cstdint>
#include <string>
#include <unordered_map>

#include "utils/sdl_ptrs.h"

namespace devilution {

void InitTTF();
void QuitTTF();

TTFFontUniquePtr LoadTTFFont(int size, const char *path);

class GlyphCache {
public:
	SDL_Surface *GetGlyph(TTF_Font *font, char32_t codepoint, SDL_Color color);

private:
	struct CacheKey {
		char32_t codepoint;
		SDL_Color color;

		bool operator==(const CacheKey &other) const
		{
			return codepoint == other.codepoint && color.r == other.color.r && color.g == other.color.g && color.b == other.color.b && color.a == other.color.a;
		}
	};

	struct CacheKeyHash {
		std::size_t operator()(const CacheKey &k) const
		{
			std::size_t h = std::hash<char32_t>{}(k.codepoint);
			h ^= std::hash<uint8_t>{}(k.color.r) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<uint8_t>{}(k.color.g) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<uint8_t>{}(k.color.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= std::hash<uint8_t>{}(k.color.a) + 0x9e3779b9 + (h << 6) + (h >> 2);
			return h;
		}
	};

	std::unordered_map<CacheKey, SDLSurfaceUniquePtr, CacheKeyHash> cache_;
};

} // namespace devilution

#endif
