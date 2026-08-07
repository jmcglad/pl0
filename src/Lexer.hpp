#ifndef LEXER_HPP
#define LEXER_HPP

#include <fstream>
#include <string>
#include <variant>
#include <cstddef>
#include <cstdint>

enum class TokenID {
    Unknown,
    EndOfFile,
    Period,
    Comma,
    Semicolon,
    Const,
    Var,
    Procedure,
    Assign,
    Call,
    Read,
    Write,
    Begin,
    End,
    If,
    Then,
    While,
    Do,
    Odd,
    Equal,
    NotEqual,
    LessThan,
    LessEqual,
    GreaterThan,
    GreaterEqual,
    Plus,
    Minus,
    Multiply,
    Divide,
    LParen,
    RParen,
    Number,
    Identifier,
};

using TokenValue = std::variant<std::monostate, std::string, std::int64_t, char>;

struct Token {
    std::size_t line;
    std::size_t column;
    TokenID id;
    TokenValue value;
};

class Lexer {
public:
    explicit Lexer(const char* path);
    explicit Lexer(const std::string& path);
    ~Lexer();
    Token next();

private:
    Token make_token(const TokenID id, TokenValue value = std::monostate()) const;
    bool fill(const std::size_t need);

    using Char = char;
    using Stream = std::basic_ifstream<Char, std::char_traits<Char>>;

    Stream stream;
    Char* buffer;
    Char* limit;
    Char* cursor;
    Char* marker;
    Char* token;
    bool eof;
    std::size_t line;
    std::size_t column;
};

#endif // LEXER_HPP
