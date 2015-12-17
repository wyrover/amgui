#ifndef AMGUI_RESOURCECACHE_HPP
#define AMGUI_RESOURCECACHE_HPP


#include <memory>
#include <unordered_map>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>


//used for declaring the hash for the font tuple
namespace std {


//hash function for the t
template <> struct hash<std::tuple<std::string, int, int>> {
    size_t operator()(const std::tuple<std::string, int, int> &t) const noexcept {
        return std::hash<std::string>()(std::get<0>(t)) ^ std::get<1>(t) ^ std::get<2>(t);
    }
};


} //namespace std


namespace amgui {


/**
    A cache of resources loaded from disk.
 */
class ResourceCache {
public:
    /**
        Loads a bitmap.
        If the bitmap is in the cache, it is returned instead.
        Otherwise, the bitmap is immediately loaded.
        The bitmap is removed from the cache when all shared pointers to it go out of scope.
        @param filename name of the bitmap to load.
        @return pointer to the loaded bitmap or null if it cannot be found.
     */
    std::shared_ptr<ALLEGRO_BITMAP> loadBitmap(const std::string &filename);

    /**
        Loads a font of the given size and flags.
        If the font with the specific size and flags is in the cache, it is returned instead.
        Otherwise, the font is immediately loaded.
        The font is removed from the cache when all shared pointers to it go out of scope.
        @param filename name of the font to load.
        @param size size of the font.
        @param flags font flags.
        @return pointer to the loaded font or null if it cannot be found.
     */
    std::shared_ptr<ALLEGRO_FONT> loadFont(const std::string &filename, int size, int flags = 0);

private:
    //bitmaps
    std::unordered_map<std::string, std::shared_ptr<ALLEGRO_BITMAP>> m_bitmaps;

    //fonts
    std::unordered_map<std::string, std::shared_ptr<ALLEGRO_FONT>> m_fonts;
};


} //namespace amgui


#endif //AMGUI_RESOURCECACHE_HPP

