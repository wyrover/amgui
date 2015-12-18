#ifndef AMGUI_WIDGET_HPP
#define AMGUI_WIDGET_HPP


#include <list>
#include <string>
#include <allegro5/allegro.h>
#include "Variant.hpp"
#include "Skin.hpp"


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
    Widget();

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
        Returns the id of the widget.
     */
    const std::string &getId() const {
        return m_id;
    }

    /**
        Sets the id of the widget.
        @param id id of the widget.
     */
    void setId(const std::string &id) {
        m_id = id;
    }

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
        Returns the root widget of this widget tree.
     */
    WidgetPtr getRoot() const;

    /**
        Returns the list of children.
     */
    const WidgetList &getChildren() const {
        return m_children;
    }

    /**
        Returns true if this widget tree contains the given widget.
     */
    bool contains(const WidgetPtr &wgt) const;

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
        return m_rect.getX();
    }

    /**
        Returns the top coordinate of the widget.
     */
    float getY() const {
        return m_rect.getY();
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
        Returns true of all widgets from this to root are enabled.
     */
    bool isEnabledTree() const;

    /**
        Sets the enabled flag.
     */
    virtual void setEnabled(bool enabled);

    /**
        Returns true if the widget currently has the mouse.
     */
    bool hasMouse() const {
        return m_mouse;
    }

    /**
        The default implementation draws the children.
        Subclasses may add the drawing code before calling the default implementation
        to draw the children.
        @param x base x coordinate to draw the widget upon.
        @param y base y coordinate to draw the widget upon.
        @param enabled if true, then the widget and all ancestors of it are enabled.
        @param highlighted if true, then the widget or an ancestor of it has the mouse.
        @param pushed if true, then the widget or an ancestor of it is pushed.
        @param selected if true, then the widget or an ancestor of it is selected.
     */
    virtual void draw(float x, float y, bool enabled, bool highlighted, bool pushed, bool selected);

    /**
        Draws this widget at the current X and Y of the widget, using the widget properties as parameters.
     */
    void draw() {
        draw(getX(), getY(), true, m_mouse, m_pushed, m_selected);
    }

    /**
        dispatches the given allegro event, to the various event methods of this widget.
        @return true if the event was used by a widget, false otherwise.
     */
    virtual bool dispatch(ALLEGRO_EVENT *event);

    /**
        Returns true if this widget has the input focus.
     */
    bool hasFocus() const {
        return _focusWidget.lock().get() == this;
    }

    /**
        Sets the focus to this widget.
        @return true if the focus was successfully set, false otherwise.
     */
    virtual bool setFocus();

    /**
        Check if the widget is in pushed state.
     */
    bool isPushed() const {
        return m_pushed;
    }

    /**
        Sets the widget to the pushed state.
     */
    virtual void setPushed(bool pushed) {
        m_pushed = pushed;
    }

    /**
        Check if the widget is in selected state.
     */
    bool isSelected() const {
        return m_selected;
    }

    /**
        Sets the widget to the selected state.
     */
    virtual void setSelected(bool selected) {
        m_selected = selected;
    }

    /**
        Returns true if drag-n-drop is in progress.
     */
    static bool isDragAndDrop() {
        return _dragAndDrop;
    }

    /**
        Returns the source of the drag and drop widget.
     */
    static const WidgetPtr &getDragAndDropSource() {
        return _dragAndDropSource;
    }

    /**
        Begins drag-n-drop, with this widget as the source of the data.
        @param draggedObject the object being dragged.
        @return true if the drag-n-drop starts successfully, false otherwise.
     */
    bool beginDragAndDrop(const Variant &draggedObject);

    /**
        Manually ends drag-n-drop, if it has been started.
        This function is invoked on mouse button up after drag-n-drop started,
        so there is no need to call it manually.
     */
    static void endDragAndDrop();

    /**
        Returns the child with the given coordinates.
     */
    WidgetPtr childFromPoint(float x, float y) const;

    /**
        Checks if the widget intersects the given point.
     */
    virtual bool intersects(float x, float y) const {
        return x >= 0 && x < getWidth() && y >= 0 && y < getHeight();
    }

    /**
        Left button down; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool leftButtonDown(int x, int y);

    /**
        right button down; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool rightButtonDown(int x, int y);

    /**
        middle button down; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool middleButtonDown(int x, int y);

    /**
        Left button up; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool leftButtonUp(int x, int y);

    /**
        right button up; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool rightButtonUp(int x, int y);

    /**
        middle button up; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool middleButtonUp(int x, int y);

    /**
        mouse enter; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool mouseEnter(int x, int y);

    /**
        mouse move; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool mouseMove(int x, int y);

    /**
        mouse leave; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool mouseLeave(int x, int y);

    /**
        mouse wheel; the default implementation dispatches the event to the child that contains the mouse.
        @return true if the event was processed, false otherwise.
     */
    virtual bool mouseWheel(int z, int w);

    /**
        Invoked when the widget lost the focus.
        @return false to prevent the widget from losing the focus, true otherwise.
     */
    virtual bool lostFocus() { return true; }

    /**
        Invoked when the widget got the focus.
     */
    virtual bool gotFocus() { return true; }

    /**
        Key down; invoked when the widget has the input focus;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool keyDown(int keycode);

    /**
        Key up; invoked when the widget has the input focus;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool keyUp(int keycode);

    /**
        character; invoked when the widget has the input focus;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool keyChar(int keycode, int unichar, int modifiers);

    /**
        unused key down; invoked when the focus widget did not process the event;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool unusedKeyDown(int keycode);

    /**
        unused key up; invoked when the focus widget did not process the event;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool unusedKeyUp(int keycode);

    /**
        unused character; invoked when the widget has the input focus;
        the default implementation dispatches the event to all children,
        stopping when the event is processed by a child.
        @return true if the event was processed, false otherwise.
     */
    virtual bool unusedKeyChar(int keycode, int unichar, int modifiers);

    /**
        Left drop; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool leftDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        right drop; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool rightDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        middle drop; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool middleDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        drag enter; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool dragEnter(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        drag move; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool dragMove(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        drag leave; the default implementation dispatches the event to children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool dragLeave(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        drag wheel; the default implementation dispatches the event to the child that contains the mouse.
        @return true if the event was processed, false otherwise.
     */
    virtual bool dragWheel(int z, int w, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource);

    /**
        Invoked on timer event.
        This implementation passes the event to all its children.
        @return true if the event was processed, false otherwise.
     */
    virtual bool timerTick(double timestamp, int64_t count);

    /**
        Used for making the widget tree occupy the smallest possible space.
        The default implementation simply passes the message to children.
        Subclasses should add the packing code after the call to the base class method.
     */
    virtual void pack();

    /**
        Used for laying out widgets according to algorithms occupied by widgets.
        The default implementation simply passes the message to children.
        Subclasses should add the layout code before the call to the base class method.
     */
    virtual void layout();

    /**
        A widget can retrieve its gui data from the given skin.
        The default implementation passes the call to its children.
     */
    virtual void setSkin(const Skin &skin);

private:
    //mainly used for debugging
    std::string m_id;

    //parent
    std::weak_ptr<Widget> m_parent;

    //children
    WidgetList m_children;

    //position into parent
    WidgetList::iterator m_it;

    //position and size
    Rect m_rect;

    //state
    bool m_visible:1;
    bool m_enabled:1;
    bool m_mouse:1;
    bool m_pushed:1;
    bool m_selected:1;

    //global state
    static std::weak_ptr<Widget> _focusWidget;
    static bool _dragAndDrop;
    static Variant _draggedObject;
    static WidgetPtr _dragAndDropSource;
    static size_t _modifiers;

    //get child with mouse
    WidgetPtr _childFromMouse() const;
};


} //namespace amgui


#endif //AMGUI_WIDGET_HPP

