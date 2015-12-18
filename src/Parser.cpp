#include <cstdlib>
#include "Parser.hpp"


namespace amgui {


/**
    The constructor.
 */
Parser::Parser(const char *text, const char *whitespace) :
    m_text(text),
    m_whitespace(whitespace),
    m_it(m_text.begin())
{
}


/**
    Parses whitespace.
 */
void Parser::parseWhitespace() {
    for(; m_it != m_text.end(); ++m_it) {
        if (m_whitespace.find(*m_it) == std::string::npos) {
            break;
        }
    }
}


/**
    Parses non-whitespace.
 */
void Parser::parseNonWhitespace() {
    for(; m_it != m_text.end(); ++m_it) {
        if (m_whitespace.find(*m_it) != std::string::npos) {
            break;
        }
    }
}


/**
    Parses a string.
 */
bool Parser::parse(std::string &str) {
    parseWhitespace();
    auto begin = m_it;
    parseNonWhitespace();
    auto end = m_it;
    str.clear();
    str.insert(str.end(), begin, end);
    return !str.empty();
}


/**
    Parses a string constant.
 */
bool Parser::parse(const char *str) {
    parseWhitespace();
    for(; m_it != m_text.end() && *str; ++m_it, ++str) {
        if (*m_it != *str) return false;
    }
    return true;
}


/**
    Parses a integer.
 */
bool Parser::parseInt(int &i, int base) {
    parseWhitespace();
    const char *t = &*m_it;
    char *p = 0;
    i = (int)strtol(t, &p, base);
    m_it += p - t;
    return p > t;
}


/**
    Parses a float.
 */
bool Parser::parse(float &f) {
    parseWhitespace();
    const char *t = &*m_it;
    char *p = 0;
    f = strtof(t, &p);
    m_it += p - t;
    return p > t;
}


/**
    Parses a double.
 */
bool Parser::parse(double &d) {
    parseWhitespace();
    const char *t = &*m_it;
    char *p = 0;
    d = strtod(t, &p);
    m_it += p - t;
    return p > t;
}


} //namespace amgui

