#pragma once

#include "LambertianMaterial.h"
#include "Logger.h"
#include "Model3d.h"
#include "Vector2.h"
#include "Vector3.h"
#include <charconv>
#include <fstream>

class Material;
class TriangleMesh;
class Model3d;

class ObjTriangle;


/**
 * Represents a token in the input string, meaning, a substring of the input string that is
 * categorized as being of a certain type.
 */
class Token
{
public:
    enum Type { String, Number, Operator, Newline, Eof }; // The type of the token
    // Maps the token type to its string representation
    static inline std::string typeStrs[5] = { "string", "number", "operator", "newline", "eof" };

    Token(Type t, const char* a, const char* b);
    Token(Type t, const std::string& s);
    Token(Type t);
    bool operator==(const Token& token);
    bool operator!=(const Token& token);

    int line, column; // The location of the token in the input
    Type type; // The type of the token
    std::string_view str; // The string that forms the token
};

/**
 * Represents a parsing error along with information describing its nature.
 */
struct ParseException
{
    ParseException(const std::string& str, int line, int col);
    ParseException(const std::string& str, const Token& t);
    ParseException(const std::string& str);
    std::string message;
};

/**
 * Represents the current state of parsing a given string. Allows for peeking the current token,
 * and expecting or accepting some token while advancing the current position in the string.
 */
class Parser
{
public:
    Parser(const std::vector<Token>& tokens);
    Token peek();
    Token next();
    bool accept(const char* s);
    bool accept(const Token& t);
    bool accept(Token::Type t);
    void expect(Token::Type t);
    void expect(const Token& token);
    bool eof();

    std::string str;
    std::vector<Token> tokens; // The stream of tokens
    int p; // The current position in the stream
};

/**
 * Optionally attempts to parse a string of any kind.
 *
 * @param parser The parser object.
 * @returns A pair of { whether we succeeded (bool), and the string that we parsed (string view) }.
 */
std::tuple<bool, std::string_view> acceptStr(Parser& parser);
bool acceptAnyCaseStr(Parser& parser, const std::string_view& str);
std::string_view expectStr(Parser& parser);
int expectInt(Parser& parser);
std::tuple<bool, int> acceptInt(Parser& parser);
real expectReal(Parser& parser);
std::tuple<bool, real> acceptReal(Parser& parser);
std::tuple<bool, int, int, int> acceptVertex(Parser& parser);
Vector3 expectVector3d(Parser& parser);
Vector2 expectVtCoordinate(Parser& parser);


std::vector<Token> tokenize(std::ifstream& file, std::string& str);