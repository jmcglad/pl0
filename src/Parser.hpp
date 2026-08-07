#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"
#include <exception>
#include <memory>
#include <string>
#include <cstdint>

class SyntaxError : public std::exception {
public:
    explicit SyntaxError(const char* what);
    explicit SyntaxError(const std::string& what);
};

class Parser {
public:
    explicit Parser(const char* path);
    explicit Parser(const std::string& path);
    std::unique_ptr<Program> parse();

private:
    std::unique_ptr<Program> parse_program();
    std::unique_ptr<Block> parse_block();
    std::unique_ptr<Stmt> parse_statement();
    std::unique_ptr<Cond> parse_condition();
    std::unique_ptr<Expr> parse_expression();
    std::unique_ptr<Expr> parse_term();
    std::unique_ptr<Expr> parse_factor();
    std::unique_ptr<Number> parse_number();
    std::unique_ptr<Identifier> parse_identifier();

    bool match(const TokenID id) const;
    bool consume(const TokenID id);
    void next();
    void error(const char* message);

    Lexer lexer;
    Token token;
};

#endif // PARSER_HPP
