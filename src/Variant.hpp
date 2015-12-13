#ifndef AMGUI_VARIANT_HPP
#define AMGUI_VARIANT_HPP


#include <memory>
#include <typeinfo>
#include <stdexcept>


namespace amgui {


/**
    Placeholder for any data type.
 */
class Variant {
public:
    /**
        Empty variant constructor.
     */
    Variant() {
    }

    /**
        Copy constructor.
        Shallow copy.
     */
    Variant(const Variant &v) : m_value(v.m_value) {}

    /**
        The move constructor.
     */
    Variant(Variant &&v) : m_value(v.m_value) {}

    /**
        constructor from data.
     */
    template <class T> Variant(const T &data) : m_value(std::make_shared<_I<T>>(data)) {}

    /**
        Returns true if the variant is empty.
     */
    bool isEmpty() const {
        return !(bool)m_value;
    }

    /**
        Checks if the variant is of the given type.
     */
    bool isType(const std::type_info &type) const {
        return isEmpty() ? false : m_value->isType(type);
    }

    /**
        Retrieves the value of the given type.
        Const version.
        @exception std::logic_error thrown if the variant is not of the given type.
     */
    template <class T> const T &get() const {
        const _I<T> *impl = dynamic_cast<const _I<T> *>(m_value.get());
        if (impl) return impl->m_data;
        throw std::logic_error("Variant type mismatch");
    }

    /**
        Same as get() (const version).
     */
    template <class T> operator const T &() const {
        return get<T>();
    }

    /**
        Retrieves the value of the given type.
        @exception std::logic_error thrown if the variant is not of the given type.
     */
    template <class T> T &get() {
        _I<T> *impl = dynamic_cast<_I<T> *>(m_value.get());
        if (impl) return impl->m_data;
        throw std::logic_error("Variant type mismatch");
    }

    /**
        Same as get().
     */
    template <class T> operator T &() {
        return get<T>();
    }

    /**
        Copy assignment from variant.
        Shallow copy.
     */
    Variant &operator = (const Variant &v) {
        m_value = v.m_value;
        return *this;
    }

    /**
        Move assignment from variant.
     */
    Variant &operator = (Variant &&v) {
        m_value = v.m_value;
        return *this;
    }

    /**
        Sets the internal data to the given value.
     */
    template <class T> Variant &operator = (const T &data) {
        m_value = std::make_shared<_I<T>>(data);
        return *this;
    }

    /**
        Sets the variant to empty.
     */
    void reset() {
        m_value.reset();
    }

private:
    //base
    struct _V {
        //destructor
        virtual ~_V() {}

        //check type
        virtual bool isType(const std::type_info &type) const = 0;
    };

    //implementation
    template <class T> struct _I : public _V {
    public:
        //data
        T m_data;

        //constructor
        _I(const T &d) : m_data(d) {
        }

        //check type
        virtual bool isType(const std::type_info &type) const {
            return typeid(T) == type;
        }
    };

    //pointer to value
    std::shared_ptr<_V> m_value;
};


} //namespace amgui


#endif //AMGUI_VARIANT_HPP
