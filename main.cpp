#include <iostream>
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "Widget.hpp"
using namespace std;
using namespace amgui;


template <class H> void print(H&& h) {
    cout << h << '\n';
}


template <class H, class ...T> void print(H&& h, T&&... t) {
    cout << h << ' ';
    print(t...);
}


class Test : public Widget {
public:
    static std::shared_ptr<Test> create(float width, float height) {
        std::shared_ptr<Test> test = std::make_shared<Test>();
        test->setSize(width, height);
        return test;
    }

    static std::shared_ptr<Test> create(const WidgetPtr &parent, float x, float y, float width, float height) {
        std::shared_ptr<Test> test = std::make_shared<Test>();
        test->setRect(x, y, width, height);
        parent->addChild(test);
        return test;
    }

    virtual void draw(float px, float py) {
        al_draw_filled_rectangle(px + getX(), py + getY(), px + getX() + getWidth(), py + getY() + getHeight(), al_map_rgb(255, 255, 255));
        al_draw_rectangle(px + getX(), py + getY(), px + getX() + getWidth(), py + getY() + getHeight(), al_map_rgb(0, 0, 0), 1);
        Widget::draw(px, py);
    }

    virtual bool leftButtonDown(int x, int y) {
        print("leftButtonDown", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::leftButtonDown(x, y);
    }

    virtual bool rightButtonDown(int x, int y) {
        print("rightButtonDown", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::rightButtonDown(x, y);
    }

    virtual bool middleButtonDown(int x, int y) {
        print("middleButtonDown", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::middleButtonDown(x, y);
    }

    virtual bool leftButtonUp(int x, int y) {
        print("leftButtonUp", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::leftButtonUp(x, y);
    }

    virtual bool rightButtonUp(int x, int y) {
        print("rightButtonUp", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::rightButtonUp(x, y);
    }

    virtual bool middleButtonUp(int x, int y) {
        print("middleButtonUp", getX(), getY(), getWidth(), getHeight(), x, y);
        return Widget::middleButtonUp(x, y);
    }
};


int main()
{
    al_init();
    al_install_mouse();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
    ALLEGRO_DISPLAY *display = al_create_display(800, 600);
    ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    WidgetPtr root = Test::create(al_get_display_width(display), al_get_display_height(display));
    WidgetPtr form1 = Test::create(root, 100, 50, 250, 200);
    WidgetPtr form2 = Test::create(root, 200, 150, 250, 200);
    WidgetPtr form3 = Test::create(root, 300, 250, 250, 200);

    WidgetPtr btn1 = Test::create(form2, 50, 40, 50, 40);
    WidgetPtr btn2 = Test::create(form2, 70, 60, 50, 40);
    WidgetPtr btn3 = Test::create(form2, 90, 80, 50, 40);

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

            case ALLEGRO_EVENT_TIMER:
                root->draw();
                al_flip_display();
                break;

            default:
                root->dispatch(&event);
                break;
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(eventQueue);
    al_destroy_display(display);

    return 0;
}
