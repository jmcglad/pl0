#include "lexer.hpp"

#include <algorithm>
#include <charconv>
#include <limits>

/*!max:re2c*/
static constexpr std::size_t SIZE = 1024;

#ifndef YYMAXFILL
#define YYMAXFILL 64
#endif

Lexer::Lexer(const char* path)
    : stream(path)
    , buffer(new Char[SIZE + YYMAXFILL])
    , limit(buffer + SIZE)
    , cursor(limit)
    , marker(limit)
    , token(limit)
    , eof(false)
    , line(1)
    , column(1)
{}

Lexer::Lexer(const std::string& path)
    : Lexer(path.c_str())
{}

Lexer::~Lexer() {
    delete[] buffer;
}

Token Lexer::next() {
    for (;;) {
    /*!re2c
        re2c:case-insensitive = 1;
        re2c:yyfill:enable = 1;
        re2c:define:YYCTYPE = "Char";
        re2c:define:YYLIMIT = "limit";
        re2c:define:YYCURSOR = "cursor";
        re2c:define:YYMARKER = "marker";
        re2c:define:YYFILL = "if (!fill(@@)) return make_token(TokenID::EndOfFile);";
        re2c:define:YYFILL:naked = 1;

        !entry {
            column += cursor - token;
            token = cursor;
        }

        *            { return make_token(TokenID::Unknown, yych); }
        "\x00"       { return make_token(TokenID::EndOfFile); }
        "\n"         { ++token; ++line; column = 1; continue; }
        [ \r\f\t\v]+ { continue; }

        "const"     { return make_token(TokenID::Const); }
        "var"       { return make_token(TokenID::Var); }
        "procedure" { return make_token(TokenID::Procedure); }
        "call"      { return make_token(TokenID::Call); }
        "begin"     { return make_token(TokenID::Begin); }
        "end"       { return make_token(TokenID::End); }
        "if"        { return make_token(TokenID::If); }
        "then"      { return make_token(TokenID::Then); }
        "while"     { return make_token(TokenID::While); }
        "do"        { return make_token(TokenID::Do); }
        "odd"       { return make_token(TokenID::Odd); }

        "."  { return make_token(TokenID::Period); }
        ","  { return make_token(TokenID::Comma); }
        ";"  { return make_token(TokenID::Semicolon); }
        ":=" { return make_token(TokenID::Assign); }
        "?"  { return make_token(TokenID::Read); }
        "!"  { return make_token(TokenID::Write); }
        "="  { return make_token(TokenID::Equal); }
        "#"  { return make_token(TokenID::NotEqual); }
        "<"  { return make_token(TokenID::LessThan); }
        "<=" { return make_token(TokenID::LessEqual); }
        ">"  { return make_token(TokenID::GreaterThan); }
        ">=" { return make_token(TokenID::GreaterEqual); }
        "+"  { return make_token(TokenID::Plus); }
        "-"  { return make_token(TokenID::Minus); }
        "*"  { return make_token(TokenID::Multiply); }
        "/"  { return make_token(TokenID::Divide); }
        "("  { return make_token(TokenID::LParen); }
        ")"  { return make_token(TokenID::RParen); }

        [0-9]+ {
            std::int64_t value;
            std::from_chars(token, cursor, value);
            return make_token(TokenID::Number, value);
        }

        [A-Za-z_][A-Za-z0-9_]* {
            std::string value(token, cursor);
            return make_token(TokenID::Identifier, value);
        }
    */
    }
}

Token Lexer::make_token(const TokenID id, TokenValue value) const {
    return Token{line, column, id, value};
}

bool Lexer::fill(const std::size_t need) {
    if (eof) {
        return false;
    }

    const auto free = token - buffer;
    if (free < need) {
        return false;
    }

    std::copy_n(token, limit - token, buffer);
    limit -= free;
    cursor -= free;
    marker = std::max(marker - free, buffer);
    token -= free;

    const auto count = stream.read(limit, free).gcount();
    if (count == std::numeric_limits<decltype(count)>::max()) {
        return false;
    }

    limit += count;
    if (limit < (buffer + SIZE)) {
        eof = true;
        std::fill_n(limit, YYMAXFILL, 0);
        limit += YYMAXFILL;
    }

    return true;
}
