#include <algorithm>
#include "Widget.hpp"


namespace amgui {


/**
    The default constructor.
 */
Widget::Widget() :
    m_visible(true),
    m_enabled(true),
    m_mouse(false),
    m_pushed(false),
    m_selected(false)
{
}


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
    Returns the root widget of this widget tree.
 */
WidgetPtr Widget::getRoot() const {
    WidgetPtr result;
    for(WidgetPtr ptr = std::const_pointer_cast<Widget>(shared_from_this()); ptr; result = ptr, ptr = ptr->getParent());
    return result;
}


/**
    Returns true if this widget tree contains the given widget.
 */
bool Widget::contains(const WidgetPtr &wgt) const {
    for(WidgetPtr temp = wgt; temp; temp = temp->getParent()) {
        if (temp.get() == this) {
            return true;
        }
    }
    return false;
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

    //if the child has the mouse, do a mouseLeave on the child,
    //because since it is removed it can no longer have the mouse
    if (widget->m_mouse) {
        widget->mouseLeave(-1, -1);
    }

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
    Returns true of all widgets from this to root are enabled.
 */
bool Widget::isEnabledTree() const {
    for(WidgetPtr ptr = std::const_pointer_cast<Widget>(shared_from_this()); ptr; ptr = ptr->getParent()) {
        if (!ptr->m_enabled) {
            return false;
        }
    }
    return true;
}


/**
    Sets the enabled flag.
 */
void Widget::setEnabled(bool enabled) {
    if (enabled == m_enabled) return;
    m_enabled = enabled;

    //special actions if the widget is disabled
    if (!enabled) {
        //if the widget is disabled while having the mouse,
        //inform it that it has lost the mouse
        if (m_mouse) {
            mouseLeave(-1, -1);
        }

        //if this or a child of it has the input focus,
        //remove the input focus
        WidgetPtr focusWidget = _focusWidget.lock();
        if (contains(focusWidget)) {
            _focusWidget.reset();
            focusWidget->lostFocus();
        }
    }
}


/**
    The default implementation draws the children.
 */
void Widget::draw(float x, float y, bool enabled, bool highlighted, bool pushed, bool selected) {
    for(auto &child : m_children) {
        if (child->m_visible) {
            child->draw(x + getX(), y + getY(), enabled && child->m_enabled, highlighted || child->m_mouse, pushed || child->m_pushed, selected || child->m_selected);
        }
    }
}


/**
    dispatches the given allegro event, to the various event methods of this widget.
    @return true if the event was used by a widget, false otherwise.
 */
bool Widget::dispatch(ALLEGRO_EVENT *event) {
    if (!m_enabled) return false;

    bool result = false;

    switch (event->type) {

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (intersects(event->mouse.x - getX(), event->mouse.y - getY())) {
                if (!_dragAndDrop) {
                    if (event->mouse.button == 1) {
                        result = leftButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (event->mouse.button == 2) {
                        result = rightButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (event->mouse.button == 3) {
                        result = middleButtonDown(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                }
            }
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (intersects(event->mouse.x - getX(), event->mouse.y - getY())) {
                if (!_dragAndDrop) {
                    if (event->mouse.button == 1) {
                        result = leftButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (event->mouse.button == 2) {
                        result = rightButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (event->mouse.button == 3) {
                        result = middleButtonUp(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                }
                else {
                    if (event->mouse.button == 1) {
                        result = leftDrop(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                    else if (event->mouse.button == 2) {
                        result = rightDrop(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                    else if (event->mouse.button == 3) {
                        result = middleDrop(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                    endDragAndDrop();
                }
            }
            break;

        case ALLEGRO_EVENT_MOUSE_AXES:
        case ALLEGRO_EVENT_MOUSE_WARPED:

            //mouse move
            if (event->mouse.dx || event->mouse.dy) {
                bool hasMouse = intersects(event->mouse.x - getX(), event->mouse.y - getY());
                if (!_dragAndDrop) {
                    if (hasMouse && m_mouse) {
                        result = mouseMove(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (hasMouse) {
                        result = mouseEnter(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                    else if (m_mouse) {
                        result = mouseLeave(event->mouse.x - getX(), event->mouse.y - getY());
                    }
                }
                else {
                    if (hasMouse && m_mouse) {
                        result = dragMove(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                    else if (hasMouse) {
                        result = dragEnter(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                    else if (m_mouse) {
                        result = dragLeave(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                    }
                }
            }

            //mouse wheel
            if (event->mouse.dz || event->mouse.dw) {
                if (!_dragAndDrop) {
                    result = mouseWheel(event->mouse.z, event->mouse.w) || result;
                }
                else {
                    result = dragWheel(event->mouse.z, event->mouse.w, _modifiers, _draggedObject, _dragAndDropSource) || result;
                }
            }

            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            if (!_dragAndDrop) {
                WidgetPtr focusWidget = _focusWidget.lock();
                if (focusWidget) {
                    result = focusWidget->keyDown(event->keyboard.keycode);
                }
                if (!result) {
                    result = unusedKeyDown(event->keyboard.keycode);
                }
            }
            break;

        case ALLEGRO_EVENT_KEY_UP:
            if (!_dragAndDrop) {
                WidgetPtr focusWidget = _focusWidget.lock();
                if (focusWidget) {
                    result = focusWidget->keyUp(event->keyboard.keycode);
                }
                if (!result) {
                    result = unusedKeyUp(event->keyboard.keycode);
                }
            }
            break;

        case ALLEGRO_EVENT_KEY_CHAR:
            if (!_dragAndDrop) {
                WidgetPtr focusWidget = _focusWidget.lock();
                if (focusWidget) {
                    result = focusWidget->keyChar(event->keyboard.keycode, event->keyboard.unichar, event->keyboard.modifiers);
                }
                if (!result) {
                    result = unusedKeyChar(event->keyboard.keycode, event->keyboard.unichar, event->keyboard.modifiers);
                }
            }
            else if (event->keyboard.modifiers != _modifiers) {
                _modifiers = event->keyboard.modifiers;
                result = dragLeave(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource);
                result = dragEnter(event->mouse.x - getX(), event->mouse.y - getY(), _modifiers, _draggedObject, _dragAndDropSource) || result;
            }
            break;

        case ALLEGRO_EVENT_TIMER:
            result = timerTick(event->timer.timestamp, event->timer.count);
            break;
    }

    return result;
}


/**
    Sets the focus to this widget.
    @return true if the focus was successfully set, false otherwise.
 */
bool Widget::setFocus() {
    //must be enabled
    if (!isEnabledTree()) return false;

    WidgetPtr prevFocusWidget = _focusWidget.lock();

    //already has the focus
    if (prevFocusWidget.get() == this) return true;

    //if there is another focus widget, tell it to lose the input focus
    if (prevFocusWidget) {
        _focusWidget.reset();
        if (!prevFocusWidget->lostFocus()) {
            _focusWidget = prevFocusWidget;
            return false;
        }
    }

    //got he focus
    _focusWidget = shared_from_this();
    gotFocus();
    return true;
}


/**
    Begins drag-n-drop.
    @return true if the drag-n-drop starts successfully, false otherwise.
 */
bool Widget::beginDragAndDrop(const Variant &draggedObject) {
    if (_dragAndDrop) return false;
    if (draggedObject.isEmpty()) return false;
    _dragAndDrop = true;
    _draggedObject = draggedObject;
    _dragAndDropSource = shared_from_this();
    getRoot()->mouseLeave(-1, -1);
    return true;
}


/**
    Ends drag-n-drop, if it has been started.
 */
void Widget::endDragAndDrop() {
    _dragAndDrop = false;
    _draggedObject.reset();
    _dragAndDropSource.reset();
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


/**
    mouse enter; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::mouseEnter(int x, int y) {
    m_mouse = true;
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->mouseEnter(x - child->getX(), y - child->getY()) : false;
}


/**
    mouse move; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::mouseMove(int x, int y) {
    WidgetPtr oldChild = _childFromMouse();
    WidgetPtr newChild = childFromPoint(x, y);
    if (newChild == oldChild) {
        return newChild && newChild->m_enabled ? newChild->mouseMove(x - newChild->getX(), y - newChild->getY()) : false;
    }
    bool ok = false;
    if (oldChild && oldChild->m_enabled) {
        ok = oldChild->mouseLeave(x - oldChild->getX(), y - oldChild->getY());
    }
    if (newChild && newChild->m_enabled) {
        ok = newChild->mouseEnter(x - newChild->getX(), y - newChild->getY()) || ok;
    }
    return ok;
}


/**
    mouse leave; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::mouseLeave(int x, int y) {
    m_mouse = false;
    WidgetPtr child = _childFromMouse();
    return child ? child->mouseLeave(x - child->getX(), y - child->getY()) : false;
}


/**
    mouse wheel; the default implementation dispatches the event to the child that contains the mouse.
    @return true if the event was processed, false otherwise.
 */
bool Widget::mouseWheel(int z, int w) {
    WidgetPtr child = _childFromMouse();
    return child ? child->mouseWheel(z, w) : false;
}


/**
    Key down; invoked when the widget has the input focus;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::keyDown(int keycode) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->keyDown(keycode)) {
            return true;
        }
    }
    return false;
}


/**
    Key up; invoked when the widget has the input focus;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::keyUp(int keycode) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->keyUp(keycode)) {
            return true;
        }
    }
    return false;
}


/**
    character; invoked when the widget has the input focus;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::keyChar(int keycode, int unichar, int modifiers) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->keyChar(keycode, unichar, modifiers)) {
            return true;
        }
    }
    return false;
}


/**
    unused key down; invoked when the focus widget did not process the event;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::unusedKeyDown(int keycode) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->unusedKeyDown(keycode)) {
            return true;
        }
    }
    return false;
}


/**
    unused key up; invoked when the focus widget did not process the event;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::unusedKeyUp(int keycode) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->unusedKeyUp(keycode)) {
            return true;
        }
    }
    return false;
}


/**
    unused character; invoked when the widget has the input focus;
    the default implementation dispatches the event to all children,
    stopping when the event is processed by a child.
    @return true if the event was processed, false otherwise.
 */
bool Widget::unusedKeyChar(int keycode, int unichar, int modifiers) {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_enabled && child->unusedKeyChar(keycode, unichar, modifiers)) {
            return true;
        }
    }
    return false;
}


/**
    Left drop; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::leftDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->leftDrop(x - child->getX(), y - child->getY(), modifiers, draggedObject, dragSource) : false;
}


/**
    right drop; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::rightDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->rightDrop(x - child->getX(), y - child->getY(), modifiers, draggedObject, dragSource) : false;
}


/**
    middle drop; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::middleDrop(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->middleDrop(x - child->getX(), y - child->getY(), modifiers, draggedObject, dragSource) : false;
}


/**
    drag enter; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::dragEnter(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    m_mouse = true;
    WidgetPtr child = childFromPoint(x, y);
    return child && child->m_enabled ? child->dragEnter(x - child->getX(), y - child->getY(), modifiers, draggedObject, dragSource) : false;
}


/**
    drag move; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::dragMove(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    WidgetPtr oldChild = _childFromMouse();
    WidgetPtr newChild = childFromPoint(x, y);
    if (newChild == oldChild) {
        return newChild && newChild->m_enabled ? newChild->dragMove(x - newChild->getX(), y - newChild->getY(), modifiers, draggedObject, dragSource) : false;
    }
    bool ok = false;
    if (oldChild && oldChild->m_enabled) {
        ok = oldChild->dragLeave(x - oldChild->getX(), y - oldChild->getY(), modifiers, draggedObject, dragSource);
    }
    if (newChild && newChild->m_enabled) {
        ok = newChild->dragEnter(x - newChild->getX(), y - newChild->getY(), modifiers, draggedObject, dragSource) || ok;
    }
    return ok;
}


/**
    drag leave; the default implementation dispatches the event to children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::dragLeave(int x, int y, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    m_mouse = false;
    WidgetPtr child = _childFromMouse();
    return child ? child->dragLeave(x - child->getX(), y - child->getY(), modifiers, draggedObject, dragSource) : false;
}


/**
    drag wheel; the default implementation dispatches the event to the child that contains the mouse.
    @return true if the event was processed, false otherwise.
 */
bool Widget::dragWheel(int z, int w, int modifiers, const Variant &draggedObject, const WidgetPtr &dragSource) {
    WidgetPtr child = _childFromMouse();
    return child ? child->dragWheel(z, w, modifiers, draggedObject, dragSource) : false;
}


/**
    Invoked on timer event.
    This implementation passes the event to all its children.
    @return true if the event was processed, false otherwise.
 */
bool Widget::timerTick(double timestamp, int64_t count) {
    bool ok = false;
    if (m_enabled) {
        for(WidgetPtr &child : m_children) {
            ok = child->timerTick(timestamp, count) || ok;
        }
    }
    return ok;
}


/**
    Used for making the widget tree occupy the smallest possible space.
    The default implementation simply passes the message to children.
    Subclasses should add the packing code after the call to the base class method.
 */
void Widget::pack() {
    for(WidgetPtr &child : m_children) {
        child->pack();
    }
}


/**
    Used for laying out widgets according to algorithms occupied by widgets.
    The default implementation simply passes the message to children.
    Subclasses should add the layout code before the call to the base class method.
 */
void Widget::layout() {
    for(WidgetPtr &child : m_children) {
        child->layout();
    }
}


//global state
std::weak_ptr<Widget> Widget::_focusWidget;
bool Widget::_dragAndDrop = false;
Variant Widget::_draggedObject;
WidgetPtr Widget::_dragAndDropSource;
size_t Widget::_modifiers = 0;


//get child with mouse
WidgetPtr Widget::_childFromMouse() const {
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto &child = *it;
        if (child->m_mouse) return child;
    }
    return WidgetPtr();
}


} //namespace amgui
