#include <cstdlib>
#include <cstring>
#include <sstream>
#include "allegro5/allegro_color.h"
#include "Skin.hpp"
#include "Parser.hpp"


namespace amgui {


static const char *_whitespace = " ,\t\n\r:\\/-";


//parse the font value into a filename, a size, and flags
static bool _parseFontValue(const char *value, std::string &filename, int &size, int &flags) {
    Parser parser(value, _whitespace);
    return parser.parse(filename, size, flags);
}


//why stricmp is not ANSI/POSIX? and why c++ doesn't have such a function?
static int _stricmp(const char *str1, const char *str2) {
    //both strings are non-null; compare characters
    if (str1 && str2) {
        for(; *str1 && *str2; ++str1, ++str2) {
            int dif = tolower(*str2) - tolower(*str1);
            if (dif) return dif;
        }
        return tolower(*str2) - tolower(*str1);
    }

    //both strings are null
    if (!str1 && !str2) {
        return 0;
    }

    //first string is null, second can't be null
    if (!str1) {
        return -1;
    }

    //first string is not null, second string is null
    return 1;
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


/**
    Returns a color at the given section and key.
    The color value can be an integer value, a hex value, an RGB triplet (e.g. 255, 12, 22), a #RRGGBB value, or a color name.
 */
ALLEGRO_COLOR Skin::getColor(const char *section, const char *key, const ALLEGRO_COLOR &defaultValue/* = ALLEGRO_COLOR()*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    Parser p(value, _whitespace);

    //try an rgb triplet
    int r, g, b;
    if (p.parse(r, g, b)) {
        return al_map_rgb(r, g, b);
    }

    //try an integer
    p.reset();
    int i;
    if (p.parse(i)) {
        return al_map_rgb((i >> 16) & 255, (i >> 8) & 255, i & 255);
    }

    //try an #RRGGBB value
    p.reset();
    if (p.parse('#') && p.parse(i, 16)) {
        return al_map_rgb((i >> 16) & 255, (i >> 8) & 255, i & 255);
    }

    //try a color name
    std::string name;
    if (p.parse(name)) {
        return al_color_name(name.c_str());
    }

    return defaultValue;
}


/**
    Returns an integer.
 */
int Skin::getInt(const char *section, const char *key, int defaultValue/* = 0*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //get the int value
    char *p = 0;
    int result = (int)strtol(value, &p, 0);

    //return the value
    return p > value ? result : defaultValue;
}


/**
    Returns an unsigned integer.
 */
unsigned int Skin::getUnsignedInt(const char *section, const char *key, unsigned int defaultValue/* = 0*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //get the int value
    char *p = 0;
    unsigned int result = (unsigned int)strtoul(value, &p, 0);

    //return the value
    return p > value ? result : defaultValue;
}


/**
    Returns a float.
 */
float Skin::getFloat(const char *section, const char *key, float defaultValue/* = 0*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //get the int value
    char *p = 0;
    float result = strtof(value, &p);

    //return the value
    return p > value ? result : defaultValue;
}


/**
    Returns a double.
 */
double Skin::getDouble(const char *section, const char *key, double defaultValue/* = 0*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //get the int value
    char *p = 0;
    double result = strtod(value, &p);

    //return the value
    return p > value ? result : defaultValue;
}


/**
    The strings 't', 'true' or '1' are recognized as the true value.
    the comparison is case insensitive.
    All other values are false.
 */
bool Skin::getBool(const char *section, const char *key, bool defaultValue/* = false*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    //compare with 'true'
    if (_stricmp(value, "true") == 0) return true;

    //compare with 't'
    if (_stricmp(value, "t") == 0) return true;

    //compare with '1'
    if (_stricmp(value, "1") == 0) return true;

    //false
    return false;
}


/**
    Reads a rectangle, i.e. the left, top, right and bottom values.
 */
Rect Skin::getRect(const char *section, const char *key, const Rect &defaultValue/* = Rect()*/) {
    //if empty, return the default value
    if (isEmpty()) return defaultValue;

    //find the value
    const char *value = al_get_config_value(m_config.get(), section, key);

    //the value is not found, so return the default value
    if (!value) return defaultValue;

    Parser p(value, _whitespace);

    float left, top, right, bottom;
    if (p.parse(left, top, right, bottom)) {
        return Rect(left, top, right, bottom);
    }

    return defaultValue;
}


} //namespace amgui

