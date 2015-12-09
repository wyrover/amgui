#ifndef AMGUI_RECT_HPP
#define AMGUI_RECT_HPP


namespace amgui {


/**
    A rectangle.
 */
class Rect {
public:
    /**
        The constructor.
     */
    Rect(float left = 0, float top = 0, float right = 0, float bottom = 0) :
        m_left(left), m_top(top), m_right(right), m_bottom(bottom)
    {
    }

    /**
        Returns the left coordinate.
     */
    float getLeft() const {
        return m_left;
    }

    /**
        Sets the left coordinate.
     */
    void setLeft(float left) {
        m_left = left;
    }

    /**
        Returns the top coordinate.
     */
    float getTop() const {
        return m_top;
    }

    /**
        Sets the top coordinate.
     */
    void setTop(float top) {
        m_top = top;
    }

    /**
        Returns the right coordinate.
     */
    float getRight() const {
        return m_right;
    }

    /**
        Sets the right coordinate.
     */
    void setRight(float right) {
        m_right = right;
    }

    /**
        Returns the bottom coordinate.
     */
    float getBottom() const {
        return m_bottom;
    }

    /**
        Sets the bottom coordinate.
     */
    void setBottom(float bottom) {
        m_bottom = bottom;
    }

    /**
        Returns right - left + 1.
     */
    float getWidth() const {
        return m_right - m_left + 1;
    }

    /**
        Sets the right to be equal to left + width - 1.
     */
    void setWidth(float width) {
        m_right = m_left + width - 1;
    }

    /**
        Returns bottom - top + 1.
     */
    float getHeight() const {
        return m_bottom - m_top + 1;
    }

    /**
        Sets the bottom to be equal to top + height - 1.
     */
    void setHeight(float height) {
        m_bottom = m_top + height - 1;
    }

    /**
        Sets the whole 4 coordinates at once.
     */
    void set(float left, float top, float right, float bottom) {
        m_left   = left  ;
        m_top    = top   ;
        m_right  = right ;
        m_bottom = bottom;
    }

    /**
        Sets the top-left corner and the size.
     */
    void setPositionAndSize(float left, float top, float width, float height) {
        m_left   = left;
        m_top    = top ;
        m_right  = m_left + width  - 1;
        m_bottom = m_top  + height - 1;
    }

    /**
        Sets the left coordinate to the given value and the right coordinate
        to a value that preserves the current width of the rectangle.
     */
    void setX(float x) {
        m_right = x + getWidth() - 1;
        m_left = x;
    }

    /**
        Sets the top coordinate to the given value and the bottom coordinate
        to a value that preserves the current height of the rectangle.
     */
    void setY(float y) {
        m_bottom = y + getHeight() - 1;
        m_top = y;
    }

    /**
        Test intersection with point.
     */
    bool intersects(float x, float y) const {
        return x >= m_left && x <= m_right && y >= m_top && y <= m_bottom;
    }

    /**
        Test intersection with rectangle.
     */
    bool intersects(const Rect &r) const {
        return r.m_left <= m_right && r.m_right >= m_left && r.m_top <= m_bottom && r.m_bottom >= r.m_top;
    }

private:
    //coordinates
    float m_left;
    float m_top;
    float m_right;
    float m_bottom;
};


} //namespace amgui


#endif //AMGUI_RECT_HPP

