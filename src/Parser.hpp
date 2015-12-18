#ifndef AMGUI_PARSER_HPP
#define AMGUI_PARSER_HPP


#include <string>


namespace amgui {


/**
    A simple parser class.
 */
class Parser {
public:
    /**
        The constructor.
     */
    Parser(const char *text, const char *whitespace);

    /**
        Check if ended.
     */
    bool isEnd() const {
        return m_it == m_text.end();
    }

    /**
        Parses whitespace.
     */
    void parseWhitespace();

    /**
        Parses non-whitespace.
     */
    void parseNonWhitespace();

    /**
        Parses a string.
     */
    bool parse(std::string &str);

    /**
        Parses a string constant.
     */
    bool parse(const char *str);

    /**
        Parses a character.
     */
    bool parse(char c) {
        parseWhitespace();
        return !isEnd() && *m_it == c ? ++m_it, true : false;
    }

    /**
        Parses a integer with a specific base.
     */
    bool parseInt(int &i, int base);

    /**
        Parses an integer with 0 base.
     */
    bool parse(int &i) {
        return parseInt(i, 0);
    }

    /**
        Parses a float.
     */
    bool parse(float &f);

    /**
        Parses a double.
     */
    bool parse(double &f);

    /**
        Parses one or more pieces of data.
        @param args arguments to parse.
        @return true if everything was parsed successfully, false if there was an error.
     */
    template <class Arg, class ...Args> bool parse(Arg&& arg, Args&&... args) {
        return parse(arg) ? parse(args...) : false;
    }

    /**
        Resets the parser to the initial state.
     */
    void reset() {
        m_it = m_text.begin();
    }

private:
    std::string m_text;
    std::string m_whitespace;
    std::string::iterator m_it;
};


} //namespace amgui


#endif //AMGUI_PARSER_HPP

