#ifndef AMGUI_WIDGET_HPP
#define AMGUI_WIDGET_HPP


#include <memory>
#include <list>
#include "Rect.hpp"


namespace amgui {


class Widget;


/**
    Pointer to widget.
 */
typedef std::shared_ptr<Widget> WidgetPtr;


/**
    List of widgets.
 */
typedef std::list<WidgetPtr> WidgetList;


/**
    Base class for widgets.
 */
class Widget : public std::enable_shared_from_this<Widget> {
public:
    /**
        The default constructor.
     */
    Widget() {
    }

    /**
        The copy constructor is deleted.
     */
    Widget(const Widget &wgt) = delete;

    /**
        The move constructor is deleted.
     */
    Widget(Widget &wgt) = delete;

    /**
        Removes this widget from its parent.
     */
    virtual ~Widget() {
        removeFromParent();
    }

    /**
        The copy assignment is deleted.
     */
    Widget &operator = (const Widget &wgt) = delete;

    /**
        The move assignment is deleted.
     */
    Widget &operator = (Widget &wgt) = delete;

    /**
        Returns a pointer to parent.
     */
    WidgetPtr getParent() const {
        return m_parent.lock();
    }

    /**
        Returns the previous sibling.
     */
    WidgetPtr getPrevSibling() const;

    /**
        Returns the next sibling.
     */
    WidgetPtr getNextSibling() const;

    /**
        Returns the first child.
     */
    WidgetPtr getFirstChild() const {
        return m_children.empty() ? WidgetPtr() : m_children.front();
    }

    /**
        Returns the last child.
     */
    WidgetPtr getLastChild() const {
        return m_children.empty() ? WidgetPtr() : m_children.back();
    }

    /**
        Returns the list of children.
     */
    const WidgetList &getChildren() const {
        return m_children;
    }

    /**
        Adds a widget as a child.
        @param widget widget to add as a child.
        @param childAfter the new child is inserted before this, if it is not null.
        @return true on success.
     */
    virtual bool addChild(const WidgetPtr &widget, const WidgetPtr &childAfter = nullptr);

    /**
        Removes a child widget.
     */
    virtual bool removeChild(const WidgetPtr &widget);

    /**
        Removes this from its parent, if there is one.
     */
    void removeFromParent();

    /**
        Returns the left coordinate of the widget.
     */
    float getX() const {
        return m_rect.getLeft();
    }

    /**
        Returns the top coordinate of the widget.
     */
    float getY() const {
        return m_rect.getTop();
    }

    /**
        Returns the width of the widget.
     */
    float getWidth() const {
        return m_rect.getWidth();
    }

    /**
        Returns the height of the widget.
     */
    float getHeight() const {
        return m_rect.getHeight();
    }

    /**
        Returns the rectangle of the widget.
     */
    const Rect &getRect() const {
        return m_rect;
    }

    /**
        Sets the rectangle of the widget.
     */
    virtual void setRect(float x, float y, float width, float height);

    /**
        Sets the rectangle of the widget.
     */
    void setRect(const Rect &r) {
        setRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
    }

    /**
        Sets the X of the widget.
     */
    void setX(float x) {
        setRect(x, getY(), getWidth(), getHeight());
    }

    /**
        Sets the Y of the widget.
     */
    void setY(float y) {
        setRect(getX(), y, getWidth(), getHeight());
    }

    /**
        Sets the width of the widget.
     */
    void setWidth(float width) {
        setRect(getX(), getY(), width, getHeight());
    }

    /**
        Sets the height of the widget.
     */
    void setHeight(float height) {
        setRect(getX(), getY(), getWidth(), getHeight());
    }

    /**
        Sets the position.
     */
    void setPosition(float x, float y) {
        setRect(x, y, getWidth(), getHeight());
    }

    /**
        Sets the size.
     */
    void setSize(float width, float height) {
        setRect(getX(), getY(), width, height);
    }

    /**
        Retrieves the visible flag.
     */
    bool isVisible() const {
        return m_visible;
    }

    /**
        Sets the visible flag.
     */
    virtual void setVisible(bool visible) {
        m_visible = visible;
    }

    /**
        Retrieves the enabled flag.
     */
    bool isEnabled() const {
        return m_enabled;
    }

    /**
        Sets the enabled flag.
     */
    virtual void setEnabled(bool enabled) {
        m_enabled = enabled;
    }

    /**
        The default implementation draws the children.
        Subclasses may add the drawing code before calling the default implementation
        to draw the children.
     */
    virtual void draw(float x, float y);

    /**
        Draws this widget at the current X and Y of the widget.
     */
    void draw() {
        draw(getX(), getY());
    }

private:
    //parent
    std::weak_ptr<Widget> m_parent;

    //children
    WidgetList m_children;

    //position into parent
    WidgetList::iterator m_it;

    //position and size
    Rect m_rect;

    //state
    bool m_visible = true;
    bool m_enabled = true;
};


} //namespace amgui


#endif //AMGUI_WIDGET_HPP

