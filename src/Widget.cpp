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
void Widget::draw(float parentX/* = 0*/, float parentY/* = 0*/) {
    for(auto &child : m_children) {
        if (child->m_visible) {
            child->draw(parentX + getX(), parentY + getY());
        }
    }
}


} //namespace amgui
