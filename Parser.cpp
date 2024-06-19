#pragma once

#include "LambertianMaterial.h"
#include "Logger.h"
#include "Model3d.h"
#include "Parser.h"
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"
#include <charconv>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <tuple>
#include <unordered_map>
#include <vector>

class Material;
class TriangleMesh;
class Model3d;

class ObjTriangle;

/**
 * Constructor. Creates a token.
 *
 * @param t The category of the token.
 * @param a A pointer to where the token starts.
 * @param b A pointer to where the next token starts.
 */
Token::Token(Type t, const char* a, const char* b) : type(t), str(a, b), line(0), column(0)
{
}

/**
 * Constructor. Creates a token.
 *
 * @param t The type of the token.
 * @param s The string that forms the token.
 */
Token::Token(Type t, const std::string& s) : type(t), str(s), line(0), column(0)
{
}

/**
 * Constructor. Creates a token.
 *
 * @param t The type of the token.
 */
Token::Token(Type t) : type(t), line(0), column(0)
{
}

/**
 * Compares the token to the given token.
 *
 * @param token The token to compare to.
 * @returns True If the strings forming the tokens are equal, and if their types are the same.
 */
bool Token::operator==(const Token& token)
{
    return type == token.type && str == token.str;
}

/**
 * Compares the token to the given token.
 *
 * @param token The token to compare to.
 * @returns True If the strings forming the tokens are non-equal, or if their types differ.
 */
bool Token::operator!=(const Token& token)
{
    return !(*this == token);
}

/**
 * Constructor. Creates a piece of information regarding a parsing error.
 *
 * @param str A string describing the error.
 * @param line The line of the error.
 * @param col The column of the error.
 */
ParseException::ParseException(const std::string& str, int line, int col)
{
    message = str + " at line " + std::to_string(line) + ", column " + std::to_string(col);
}


/**
 * Constructor. Creates a ParseException.
 *
 * @param str A string describing the error.
 * @param t The token that caused the error.
 */
ParseException::ParseException(const std::string& str, const Token& t)
{
    message = str + " at line " + std::to_string(t.line) + ", column " + std::to_string(t.column);
}

/**
 * Constructor.
 *
 * @param str A string describing the error.
 */
ParseException::ParseException(const std::string& str)
{
    message = str;
}

/**
 * Constructor.
 *
 * @param tokens A vector of tokens of a tokenized string.
 */
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), p(0)
{
}

/**
 * Returns the token at the current position in the string.
 *
 * @returns The token.
 */
Token Parser::peek()
{
    return tokens[p];
}

/**
 * Returns the token at the current position in the string, and advances the string pointer.
 *
 * @returns The token at the current position.
 */
Token Parser::next()
{
    return tokens[p++];
}

/**
 * Checks if the string contents of the current token equals the argument, and advances to
 * the next token if it does.
 *
 * @returns True if the string matched.
 */
bool Parser::accept(const char* s)
{
    return tokens[p].str == s ? ++p : false;
}

/**
 * Checks if the current token in the parsing stream equals the argument, and advances
 * the stream if it does.
 *
 * @returns True if the token matched.
 */
bool Parser::accept(const Token& t)
{
    return tokens[p] == t ? ++p : false;
}

/**
 * Checks if the type of the current token in the parsing stream equals that of the argument,
 * and advances the stream if it does.
 *
 * @returns True if the type matched.
 */
bool Parser::accept(Token::Type t)
{
    return tokens[p].type == t ? ++p : false;
}

/**
 * Checks that the type of the current token in the stream matches that of the
 * argument, and advances the stream if so.
 *
 * @throws ParseException if the tokens don't match.
 */
void Parser::expect(Token::Type t)
{
    if(tokens[p].type != t)
        throw ParseException("Expected " + Token::typeStrs[t], tokens[p]);
    p++;
}

/**
 * Checks that the current token in the stream matches that of the argument, and advances
 * the stream if so.
 *
 * @throws ParseException if the tokens don't match.
 */
void Parser::expect(const Token& token)
{
    if(tokens[p] != token)
        throw ParseException("Expected " + Token::typeStrs[token.type] + " " + std::string(token.str), tokens[p]);
    p++;
}

/**
 * Checks if the stream has reached the end.
 *
 * @returns True if we are at EOF.
 */
bool Parser::eof()
{
    return p == tokens.size();
}

/**
 * Optionally attempts to parse a string of any kind.
 *
 * @param parser The parser object.
 * @returns A pair of { whether we succeeded (bool), and the string that we parsed (string view) }.
 */
std::tuple<bool, std::string_view> acceptStr(Parser& parser)
{
    if(parser.peek().type != Token::String)
        return { false, "" };

    return { true, parser.next().str };
}

/**
 * Attempts to parse the given string.
 *
 * @param parser The parser object.
 * @param parser The string we accept.
 * @returns Whether the string was parsed.
 */
bool acceptAnyCaseStr(Parser& parser, const std::string_view& str)
{
    if(parser.peek().type != Token::String)
        return false;

    auto a = parser.peek().str;
    if(!std::equal(a.begin(), a.end(), str.begin(), str.end(), [](char a, char b) { return tolower(a) == tolower(b); }))
        return false;

    parser.next();
    return true;
}

/**
 * Unconditionally parses an alphanumeric string without whitespace.
 *
 * @throws A ParseException if the string isn't alphanumeric.
 * @param parser The parser object.
 * @returns A string_view of the string.
 */
std::string_view expectStr(Parser& parser)
{
    if(parser.peek().type != Token::String)
        throw ParseException("Alphanumeric string expected", parser.peek().line, parser.peek().column);
    return parser.next().str;
}

/**
 * Unconditionally parses an integer.
 *
 * @throws A ParseException if an integer point number was unable to be parsed.
 * @param parser The parser object.
 * @returns The integer.
 */
int expectInt(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        throw ParseException("Integer expected", parser.peek());

    int d = 0;
    auto st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);
    if(res.ec == std::errc::invalid_argument)
        throw ParseException("Integer expected", parser.peek());

    return d;
}

/**
 * Optionally parses an integer.
 *
 * @param parser The parser object.
 * @returns The integer.
 */
std::tuple<bool, int> acceptInt(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    int d = 0;
    auto st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return { true, d };
}

/**
 * Unconditionally parses a floating point number.
 *
 * @throws A ParseException if a floating point number was unable to be parsed.
 * @param parser The parser object.
 * @returns The real number.
 */
double expectReal(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        throw ParseException("Floating point expected", parser.peek());

    double d = 0;
    auto st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return d;
}

/**
 * Attempts to parse a floating point number.
 *
 * @param parser The parser object.
 * @returns A tuple of { success, real } where success is if the parsing was successful and
 *          along with the real number parsed.
 */
std::tuple<bool, double> acceptReal(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    double d = 0;
    const auto& st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return { true, d };
}

/**
 * Attempts to parse a v/t/n face element.
 *
 * @throws ParseException If an element was encountered but it contained badly formed data.
 * @param parser The parser object.
 * @returns A tuple of success/v/t/n indices. Success is true if the parsing succeeded, and the
 *          remaining entires are the vertex, texture and normal indices, which can be zero if
 *          it was not supplied.
 */
std::tuple<bool, int, int, int> acceptVertex(Parser& parser)
{
    auto [success, n1] = acceptInt(parser);
    if(!success)
        return { false, 0, 0, 0 };

    if(!parser.accept("/")) // Format is x
        return { true, n1, 0, 0 };

    if(parser.accept("/")) // Format is x//y
        return { true, n1, 0, expectInt(parser) };

    int n2 = expectInt(parser); // Format is x/y..

    if(!parser.accept("/"))
        return { true, n1, n2, 0 };

    // Format is x/y/z
    return { true, n1, n2, expectInt(parser) };
}

/**
 * Parses a 3d vector consisting of floating point numbers.
 *
 * @throws ParseException if a 3d vector was unable to be parsed.
 * @param parser The parser object.
 * @returns A 3d vector.
 */
Vector3 expectVector3d(Parser& parser)
{
    real arr[3];
    for(int i = 0; i < 3; i++)
        arr[i] = expectReal(parser);
    return Vector3(arr[0], arr[1], arr[2]);
}

/**
 * Parses a texture coordinate.
 *
 * @throws ParseException if a texture coordinate was unable to be parsed.
 * @param parser The parser object.
 * @returns A 2d vector of the coordinates. Depending on the dimensionality of the coordinate
 *          one or several of the trailing entries could be zero.
 */
Vector2 expectVtCoordinate(Parser& parser)
{
    double arr[3] = { 0, 0, 0 };
    bool hadSuccess = false;
    for(int i = 0; i < 3; i++)
    {
        auto [success, d] = acceptReal(parser);
        if(success)
        {
            arr[i] = d;
            hadSuccess = true;
        }
    }
    if(!hadSuccess)
        throw ParseException("Bad texture coordinate");
    return Vector2(arr[0], arr[1]);
}

/**
 * Turns the contents of a text file into a vector of Tokens.
 *
 * @throws ParseException if an unknown token was encountered.
 * @param file The file to tokenize.
 * @returns A vector of Token objects.
 */
std::vector<Token> tokenize(std::ifstream& file, std::string& str)
{
    //Timer t1;
    //t1.Reset();

    // Before tokenizing, remove any comments
    int i = 0;
    for(std::string ln; !file.eof(); str += '\n')
        for(std::getline(file, ln), i = 0; i < ln.size() && ln[i] != '#'; i++)
            str += ln[i];

    std::vector<Token> v;
    int line = 1, col = 1, p = 0;

    // Adds a token to the token stream
    auto addToken = [&v, &line, &col, &str](Token::Type t, int a, int b)
        {
            for(auto it = str.begin()+a; it < str.begin()+b; it++)
            {
                if(*it == '\n')
                    line++, col = 1;
                else
                    col++;
            }
            Token token(t, str.c_str()+a, str.c_str()+b);
            token.line = line;
            token.column = col;
            v.push_back(token);
        };

        // Skips every character until the next token
    auto skipspace = [&p, &str]()
        {
            while(p < str.length() && str[p] == ' ' || str[p] == '\t')
                p++;
        };

        // Returns current character in the string
    auto peek = [&p, &str]() -> char
        {
            return p < str.size() ? str[p] : 0;
        };

        // Advances to the next character in the string
    auto next = [&p, &str]() -> char
        {
            return p < str.size() ? str[p++] : 0;
        };

        // Optionally advances to the next character in the string if the given character matches
    auto accept = [&p, &str](char c) -> bool
        {
            return (p < str.size() && str[p] == c) ? ++p : false;
        };

    while(p < str.size())
    {
        skipspace();
        auto c = peek();
        if(c == '/' || c == '{' || c == '}' || c == ':') // Operator
        {
            addToken(Token::Operator, p, p+1);
            p++;
        }

        else if(std::isdigit(c) || c == '-' || c == '.') // Number
        {
            int d = p;
            // Not the best way of parsing a number ..
            for(auto c = peek(); std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-'; c = peek())
                p++;

            double D = 0;
            auto res = std::from_chars(str.c_str()+d, str.c_str() + p, D);
            if(res.ec == std::errc::invalid_argument)
                throw ParseException("Couldn't parse floating point number \"" + str.substr(d, p-d) + "\"", line, col);

            addToken(Token::Number, d, p);
        }
        else if(std::isalpha(c)) // Identifier
        {
            // Another way we could do this is to make a token resolve to a string type as a last fallback after
            // trying every other type. This would solve stuff like "mttllib ./blah.mtl" which tries to parse
            // ./blah.mtl as a floating point number, and then just throwing outright, instead of trying to
            // interpret it as a string. Of course, this wouldn't solve other types of contextual ambiguities
            int d = p;
            while(!std::isspace(peek()))
                p++;
            addToken(Token::String, d, p);
        }
        else if(c == '\n') // Endline
        {
            int d = p;
            addToken(Token::Newline, d, ++p);
        }
        else if(accept('\r'))
        {
            int d = p;
            addToken(Token::Newline, d, ++p);
        }
        else
            throw ParseException(std::string("Couldn't parse character: \"") + peek() + "\"");
    }
    addToken(Token::Eof, str.size(), str.size());
    //logger.Box(std::to_string(t1.GetTime()));
    return v;
}
