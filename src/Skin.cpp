#include <sstream>
#include "Skin.hpp"


namespace amgui {


//parse the font value into a filename, a size, and flags
static bool _parseFontValue(const char *value, std::string &filename, int &size, int &flags) {
    //TODO
    return true;
}


/**
    Loads a skin from a disk file.
    @param filename name of the Allegro config file to load for the skin.
 */
Skin::Skin(const char *filename) :
    m_config(al_load_config_file(filename), al_destroy_config)
{
}


/**
    Searches the internal config for a bitmap filename which corresponds to the given section and key.
    If the filename is found, then the bitmap is loaded (or retrieved from the resource cache),
    otherwise null is returned.
 */
std::shared_ptr<ALLEGRO_BITMAP> Skin::getBitmap(const char *section, const char *key, const std::shared_ptr<ALLEGRO_BITMAP> &defaultValue/* = nullptr*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *filename = al_get_config_value(m_config.get(), section, key);

    //the filename is not found, so return the default value
    if (!filename) return defaultValue;

    //get the resource from the resource cache
    auto result = m_resourceCache.loadBitmap(filename);

    //return either the result or the default value
    return result ? result : defaultValue;
}


/**
    Searches the internal config for a font filename which corresponds to the given section and key.
    If the filename is found, then the font is loaded (or retrieved from the resource cache),
    otherwise the default value is returned.
    The font filename can be followed by an optional size value (default is 12) and an optional flags value (the default is 0).
 */
std::shared_ptr<ALLEGRO_FONT> Skin::getFont(const char *section, const char *key, const std::shared_ptr<ALLEGRO_FONT> &defaultValue/* = nullptr*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //find the size and flags
    std::string filename;
    int size = 12, flags = 0;
    if (!_parseFontValue(value, filename, size, flags)) return defaultValue;

    //get the resource from the resource cache
    auto result = m_resourceCache.loadFont(filename, size, flags);

    //return either the result or the default value
    return result ? result : defaultValue;
}


} //namespace amgui

