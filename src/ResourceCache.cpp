#include <sstream>
#include "ResourceCache.hpp"


namespace amgui {


//create the font id
static std::string _makeFontId(const std::string &filename, int size, int flags) {
    std::stringstream stream;
    stream << filename << '_' << size << '_' << flags;
    return stream.str();
}


//load a bitmap
std::shared_ptr<ALLEGRO_BITMAP> ResourceCache::loadBitmap(const std::string &filename) {
    //find a bitmap in the cache
    auto cached = m_bitmaps.find(filename);

    //if found, return it
    if (cached != m_bitmaps.end()) {
        return cached->second;
    }

    //load the bitmap
    ALLEGRO_BITMAP *bitmap = al_load_bitmap(filename.c_str());

    //the bitmap was not loaded
    if (!bitmap) {
        return nullptr;
    }

    //create and returned a shared pointer with a destructor that removes it from the cache
    std::shared_ptr<ALLEGRO_BITMAP> result{bitmap, [=](ALLEGRO_BITMAP *bmp) {
        m_bitmaps.erase(m_bitmaps.find(filename));
        al_destroy_bitmap(bmp);
    }};

    //put the bitmap in the cache
    m_bitmaps[filename] = result;

    //return the result
    return result;
}


//loads a font
std::shared_ptr<ALLEGRO_FONT> ResourceCache::loadFont(const std::string &filename, int size, int flags/* = 0*/) {
    //find a font in the cache
    auto id = _makeFontId(filename, size, flags);
    auto cached = m_fonts.find(id);

    //if found, return it
    if (cached != m_fonts.end()) {
        return cached->second;
    }

    //load the font
    ALLEGRO_FONT *font = al_load_font(filename.c_str(), size, flags);

    //the font was not loaded
    if (!font) {
        return nullptr;
    }

    //create and returned a shared pointer with a destructor that removes it from the cache
    std::shared_ptr<ALLEGRO_FONT> result{font, [=](ALLEGRO_FONT *fnt) {
        m_fonts.erase(m_fonts.find(id));
        al_destroy_font(fnt);
    }};

    //put the font in the cache
    m_fonts[id] = result;

    //return the result
    return result;
}


} //namespace amgui

