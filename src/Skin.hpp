#ifndef AMGUI_SKIN_HPP
#define AMGUI_SKIN_HPP


#include "ResourceCache.hpp"
#include "Rect.hpp"


namespace amgui {


/**
    A skin is nothing more than a wrapper to an Allegro config file which can be used to specify data for a gui to load.
 */
class Skin {
public:
    /**
        Loads a skin from a disk file.
        @param filename name of the Allegro config file to load for the skin.
     */
    Skin(const char *filename);

    /**
        Returns true if the config is empty, or if it is not loaded.
     */
    bool isEmpty() const {
        return !(bool)m_config;
    }

    /**
        Searches the internal config for a bitmap filename which corresponds to the given section and key.
        If the filename is found, then the bitmap is loaded (or retrieved from the resource cache),
        otherwise the default value is returned.
     */
    std::shared_ptr<ALLEGRO_BITMAP> getBitmap(const char *section, const char *key, const std::shared_ptr<ALLEGRO_BITMAP> &defaultValue = nullptr);

    /**
        Searches the internal config for a font filename which corresponds to the given section and key.
        If the filename is found, then the font is loaded (or retrieved from the resource cache),
        otherwise the default value is returned.
        The font filename can be followed by an optional size value (default is 12) and an optional flags value (the default is 0).
        For example: myfont.ttf, 12, 0.
     */
    std::shared_ptr<ALLEGRO_FONT> getFont(const char *section, const char *key, const std::shared_ptr<ALLEGRO_FONT> &defaultValue = nullptr);

    /**
        Returns a color at the given section and key.
        The color value can be an integer value, a hex value, an RGB triplet (e.g. 255, 12, 22), a #RRGGBB value, or a color name.
     */
    ALLEGRO_COLOR getColor(const char *section, const char *key, const ALLEGRO_COLOR &defaultValue = ALLEGRO_COLOR());

    /**
        Returns an integer.
     */
    int getInt(const char *section, const char *key, int defaultValue = 0);

    /**
        Returns an unsigned integer.
     */
    unsigned int getUnsignedInt(const char *section, const char *key, unsigned int defaultValue = 0);

    /**
        Returns a float.
     */
    float getFloat(const char *section, const char *key, float defaultValue = 0);

    /**
        Returns a double.
     */
    double getDouble(const char *section, const char *key, double defaultValue = 0);

    /**
        Returns a string.
     */
    const char *getString(const char *section, const char *key, const char *defaultValue = nullptr) {
        return isEmpty() ? defaultValue : al_get_config_value(m_config.get(), section, key);
    }

    /**
        The strings 't', 'true' or '1' are recognized as the true value.
        the comparison is case insensitive.
        All other values are false.
     */
    bool getBool(const char *section, const char *key, bool defaultValue = false);

    /**
        Reads a rectangle, i.e. the left, top, right and bottom values.
     */
    Rect getRect(const char *section, const char *key, const Rect &defaultValue = Rect());

private:
    //bitmaps, fonts etc are stored here
    ResourceCache m_resourceCache;

    //allegro config
    std::shared_ptr<ALLEGRO_CONFIG> m_config;
};


} //namespace amgui


#endif //AMGUI_SKIN_HPP

