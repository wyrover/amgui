#include <iostream>
#include "allegro5/allegro.h"
#include "Widget.hpp"
using namespace std;
using namespace amgui;


int main()
{
    al_init();
    al_install_mouse();
    al_install_keyboard();

    ALLEGRO_DISPLAY *display = al_create_display(800, 600);

    ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    bool loop = true;
    while (loop)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);
        switch (event.type)
        {
            case ALLEGRO_EVENT_KEY_DOWN:
                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ESCAPE:
                        loop = false;
                        break;
                }
                break;
        }
    }

    al_destroy_event_queue(eventQueue);
    al_destroy_display(display);

    return 0;
}
