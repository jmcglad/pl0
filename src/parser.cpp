#include "parser.hpp"

#include <fmt/format.h>
#include <utility>
#include <variant>

SyntaxError::SyntaxError(const char* what)
    : std::exception(what)
{}

SyntaxError::SyntaxError(const std::string& what)
    : SyntaxError(what.c_str())
{}

Parser::Parser(const char* path)
    : lexer(path)
    , token()
{}

Parser::Parser(const std::string& path)
    : Parser(path.c_str())
{}

std::unique_ptr<Program> Parser::parse() {
    token = lexer.next();

    auto program = parse_program();

    if (!match(TokenID::EndOfFile)) {
        error("junk at end of file");
    }

    return program;
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();

    program->block = parse_block();

    if (!consume(TokenID::Period)) {
        error("missing '.' at end of program");
    }

    return program;
}

std::unique_ptr<Block> Parser::parse_block() {
    auto block = std::make_unique<Block>();

    if (consume(TokenID::Const)) {
        do {
            auto constant = std::make_unique<Constant>();

            if (match(TokenID::Identifier)) {
                constant->identifier = parse_identifier();
            } else {
                error("identifier expected for const name");
            }

            if (!consume(TokenID::Equal)) {
                error("missing '=' after const identifier");
            }

            if (match(TokenID::Number)) {
                constant->number = parse_number();
            } else {
                error("number expected for const value");
            }

            block->constants.push_back(std::move(constant));
        } while (consume(TokenID::Comma));

        if (!consume(TokenID::Semicolon)) {
            error("missing ';' after const declarations");
        }
    }

    if (consume(TokenID::Var)) {
        do {
            auto variable = std::make_unique<Variable>();

            if (match(TokenID::Identifier)) {
                variable->identifier = parse_identifier();
            } else {
                error("identifier expected for var name");
            }

            block->variables.push_back(std::move(variable));
        } while (consume(TokenID::Comma));

        if (!consume(TokenID::Semicolon)) {
            error("missing ';' after var declarations");
        }
    }

    while (consume(TokenID::Procedure)) {
        auto procedure = std::make_unique<Procedure>();

        if (match(TokenID::Identifier)) {
            procedure->identifier = parse_identifier();
        } else {
            error("identifier expected for procedure name");
        }

        if (!consume(TokenID::Semicolon)) {
            error("missing ';' after procedure name");
        }

        procedure->block = parse_block();

        if (!consume(TokenID::Semicolon)) {
            error("missing ';' after procedure block");
        }

        block->procedures.push_back(std::move(procedure));
    }

    block->statement = parse_statement();

    return block;
}

std::unique_ptr<Stmt> Parser::parse_statement() {
    if (match(TokenID::Identifier)) {
        auto statement = std::make_unique<AssignmentStmt>();

        statement->left = parse_identifier();

        if (!consume(TokenID::Assign)) {
            error("missing ':=' after identifier");
        }

        statement->right = parse_expression();

        return statement;
    } else if (consume(TokenID::Call)) {
        auto statement = std::make_unique<CallStmt>();

        if (match(TokenID::Identifier)) {
            statement->target = parse_identifier();
        } else {
            error("identifier expected after keyword \"call\"");
        }

        return statement;
    } else if (consume(TokenID::Read)) {
        auto statement = std::make_unique<ReadStmt>();

        if (match(TokenID::Identifier)) {
            statement->identifier = parse_identifier();
        } else {
            error("identifier expected after '?'");
        }

        return statement;
    } else if (consume(TokenID::Write)) {
        auto statement = std::make_unique<WriteStmt>();
        statement->expression = parse_expression();
        return statement;
    } else if (consume(TokenID::Begin)) {
        auto statement = std::make_unique<BeginStmt>();

        do {
            statement->children.push_back(parse_statement());
        } while (consume(TokenID::Semicolon));

        if (!consume(TokenID::End)) {
            error("expected keyword \"end\" after begin statement");
        }

        return statement;
    } else if (consume(TokenID::If)) {
        auto statement = std::make_unique<IfStmt>();

        statement->condition = parse_condition();

        if (!consume(TokenID::Then)) {
            error("missing keyword \"then\" after if condition");
        }

        statement->statement = parse_statement();

        return statement;
    } else if (consume(TokenID::While)) {
        auto statement = std::make_unique<WhileStmt>();

        statement->condition = parse_condition();

        if (!consume(TokenID::Do)) {
            error("missing keyword \"do\" after while condition");
        }

        statement->statement = parse_statement();

        return statement;
    } else {
        error("invalid statement");
        return nullptr;
    }
}

std::unique_ptr<Cond> Parser::parse_condition() {
    if (consume(TokenID::Odd)) {
        auto condition = std::make_unique<OddCond>();
        condition->right = parse_expression();
        return condition;
    } else {
        std::unique_ptr<BinaryCond> condition;

        auto left = parse_expression();

        if (consume(TokenID::Equal)) {
            condition = std::make_unique<EqualCond>();
        } else if (consume(TokenID::NotEqual)) {
            condition = std::make_unique<NotEqualCond>();
        } else if (consume(TokenID::LessThan)) {
            condition = std::make_unique<LessThanCond>();
        } else if (consume(TokenID::LessEqual)) {
            condition = std::make_unique<LessEqualCond>();
        } else if (consume(TokenID::GreaterThan)) {
            condition = std::make_unique<GreaterThanCond>();
        } else if (consume(TokenID::GreaterEqual)) {
            condition = std::make_unique<GreaterEqualCond>();
        } else {
            error("expected a condition operator");
            return nullptr;
        }

        condition->left = std::move(left);
        condition->right = parse_expression();

        return condition;
    }
}

std::unique_ptr<Expr> Parser::parse_expression() {
    std::unique_ptr<Expr> expression;

    if (consume(TokenID::Plus)) {
        expression = parse_term();
    } else if (consume(TokenID::Minus)) {
        auto negation = std::make_unique<NegationExpr>();
        negation->right = parse_term();
        expression = std::move(negation);
    } else {
        expression = parse_term();
    }

    for (;;) {
        std::unique_ptr<BinaryExpr> sub_expression;

        if (consume(TokenID::Plus)) {
            sub_expression = std::make_unique<AdditionExpr>();
        } else if (consume(TokenID::Minus)) {
            sub_expression = std::make_unique<SubtractionExpr>();
        } else {
            break;
        }

        sub_expression->left = std::move(expression);
        sub_expression->right = parse_term();
        expression = std::move(sub_expression);
    }

    return expression;
}

std::unique_ptr<Expr> Parser::parse_term() {
    auto term = parse_factor();

    for (;;) {
        std::unique_ptr<BinaryExpr> sub_term;

        if (consume(TokenID::Multiply)) {
            sub_term = std::make_unique<MultiplicationExpr>();
        } else if (consume(TokenID::Divide)) {
            sub_term = std::make_unique<DivisionExpr>();
        } else {
            break;
        }

        sub_term->left = std::move(term);
        sub_term->right = parse_factor();
        term = std::move(sub_term);
    }

    return term;
}

std::unique_ptr<Expr> Parser::parse_factor() {
    if (match(TokenID::Identifier)) {
        return parse_identifier();
    } else if (match(TokenID::Number)) {
        return parse_number();
    } else if (consume(TokenID::LParen)) {
        auto expression = parse_expression();

        if (!consume(TokenID::RParen)) {
            error("missing ')' after expression");
        }

        return expression;
    } else {
        error("invalid factor");
        return nullptr;
    }
}

std::unique_ptr<Number> Parser::parse_number() {
    auto number = std::make_unique<Number>();
    number->value = std::get<std::int64_t>(token.value);
    next();
    return number;
}

std::unique_ptr<Identifier> Parser::parse_identifier() {
    auto identifier = std::make_unique<Identifier>();
    identifier->name = std::get<std::string>(token.value);
    next();
    return identifier;
}

bool Parser::match(const TokenID id) const {
    return id == token.id;
}

bool Parser::consume(const TokenID id) {
    if (match(id)) {
        next();
        return true;
    }

    return false;
}

void Parser::next() {
    token = lexer.next();
}

void Parser::error(const char* message) {
    auto what = fmt::format("{}:{}: {}", token.line, token.column, message);
    throw SyntaxError(what);
}
