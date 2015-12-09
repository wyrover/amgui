#include <algorithm>
#include "Widget.hpp"


namespace amgui {


/**
    Returns the previous sibling.
 */
WidgetPtr Widget::getPrevSibling() const {
    WidgetPtr parent = m_parent.lock();
    if (parent && m_it != parent->m_children.begin()) {
        auto it = m_it;
        --it;
        return *it;
    }
    return WidgetPtr();
}


/**
    Returns the next sibling.
 */
WidgetPtr Widget::getNextSibling() const {
    WidgetPtr parent = m_parent.lock();
    if (parent) {
        auto it = m_it;
        ++it;
        return it != parent->m_children.end() ? *it : WidgetPtr();
    }
    return WidgetPtr();
}


/**
    Adds a widget as a child.
    @param widget widget to add as a child.
    @param childAfter the new child is inserted before this, if it is not null.
    @return true on success.
 */
bool Widget::addChild(const WidgetPtr &widget, const WidgetPtr &childAfter/* = nullptr*/) {
    //can't be null
    if (!widget) return false;

    //can't be the same as this
    if (widget.get() == this) return false;

    //can't be the same as he childAfter
    if (widget == childAfter) return false;

    //can't have a parent
    if (widget->m_parent.lock()) return false;

    //if the childAfter is specified, then its parent must be this
    if (childAfter && childAfter->m_parent.lock().get() != this) return false;

    //insert the child
    widget->m_parent = shared_from_this();
    widget->m_it = m_children.insert(childAfter ? childAfter->m_it : m_children.end(), widget);

    //success
    return true;
}


/**
    Removes a child widget.
 */
bool Widget::removeChild(const WidgetPtr &widget) {
    //can't be null
    if (!widget) return false;

    //must be child
    if (widget->m_parent.lock().get() != this) return false;

    //remove the child
    m_children.erase(widget->m_it);
    widget->m_parent.reset();

    //success
    return true;
}


/**
    Removes this widget from its parent.
 */
void Widget::removeFromParent() {
    WidgetPtr parent = m_parent.lock();
    if (parent) {
        parent->removeChild(shared_from_this());
    }
}


/**
    Sets the rectangle of the widget.
 */
void Widget::setRect(float x, float y, float width, float height) {
    m_rect.setPositionAndSize(x, y, std::max(width, 0.f), std::max(height, 0.f));
}


/**
    The default implementation draws the children.
 */
void Widget::draw(float x, float y) {
    for(auto &child : m_children) {
        if (child->m_visible) {
            child->draw(x + getX(), y + getY());
        }
    }
}


/**
    dispatches the given allegro event, to the various event methods of this widget.
    @return true if the event was used by a widget, false otherwise.
 */
bool Widget::dispatch(ALLEGRO_EVENT *event) {
    if (!m_enabled) return false;

    switch (event->type) {

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (intersects(event->mouse.x - getX(), event->mouse.y - getY())) {
                if (!_dragAndDrop) {
                    if (event->mouse.button == 1) {
                        return leftButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    if (event->mouse.button == 2) {
                        return rightButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    if (event->mouse.button == 3) {
                        return middleButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                }
                else {
                    //TODO
                }
            }
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (intersects(event->mouse.x - getX(), event->mouse.y - getY())) {
                if (!_dragAndDrop) {
                    if (event->mouse.button == 1) {
                        return leftButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    if (event->mouse.button == 2) {
                        return rightButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    if (event->mouse.button == 3) {
                        return middleButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                }
                else {
                    //TODO
                }
            }
            break;
    }

    return false;
}


/**
    Returns the child with the given coordinates.
 */
WidgetPtr Widget::childFromPoint(float x, float y) const {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_visible && child->intersects(x - child->getX(), y - child->getY())) {
            return child;
        }
    }
    return WidgetPtr();
}


/**
    Left button down; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::leftButtonDown(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->leftButtonDown(x - child->getX(), y - child->getY()) : false;
}


/**
    right button down; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::rightButtonDown(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->rightButtonDown(x - child->getX(), y - child->getY()) : false;
}


/**
    middle button down; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::middleButtonDown(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->middleButtonDown(x - child->getX(), y - child->getY()) : false;
}


/**
    Left button up; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::leftButtonUp(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->leftButtonUp(x - child->getX(), y - child->getY()) : false;
}


/**
    right button up; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::rightButtonUp(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->rightButtonUp(x - child->getX(), y - child->getY()) : false;
}


/**
    middle button up; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::middleButtonUp(int x, int y) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->middleButtonUp(x - child->getX(), y - child->getY()) : false;
}


//global state
bool Widget::_dragAndDrop = false;


} //namespace amgui
