#ifndef AMGUI_SKIN_HPP
#define AMGUI_SKIN_HPP


#include "ResourceCache.hpp"


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

private:
    //bitmaps, fonts etc are stored here
    ResourceCache m_resourceCache;

    //allegro config
    std::shared_ptr<ALLEGRO_CONFIG> m_config;
};


} //namespace amgui


#endif //AMGUI_SKIN_HPP

