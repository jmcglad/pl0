#include "evaluator.hpp"

#include <iostream>
#include <ranges>
#include <fmt/format.h>

EvaluationError::EvaluationError(const char* what)
    : std::exception(what)
{}

EvaluationError::EvaluationError(const std::string& what)
    : EvaluationError(what.c_str())
{}

Evaluator::Evaluator()
    : scopes{Scope()}
    , values()
    , condition_result(false)
{}

void Evaluator::visit(Program& program) {
    program.block->accept(*this);
}

void Evaluator::visit(Block& block) {
    for (auto& constant : block.constants) {
        constant->accept(*this);
    }

    for (auto& variable : block.variables) {
        variable->accept(*this);
    }

    for (auto& procedure : block.procedures) {
        scopes.back().procedures.emplace(
            procedure->identifier->name,
            *procedure
        );
    }

    block.statement->accept(*this);
}

void Evaluator::visit(Constant& constant) {
    scopes.back().constants.emplace(
        constant.identifier->name,
        constant.number->value
    );
}

void Evaluator::visit(Variable& variable) {
    scopes.back().variables.emplace(variable.identifier->name, 0);
}

void Evaluator::visit(Procedure& procedure) {
    scopes.emplace_back();
    procedure.block->accept(*this);
    scopes.pop_back();
}

void Evaluator::visit(AssignmentStmt& statement) {
    for (auto& scope : scopes | std::views::reverse) {
        auto it = scope.variables.find(statement.left->name);
        if (it != scope.variables.end()) {
            statement.right->accept(*this);
            it->second = values.top();
            return;
        }
    }

    auto what = fmt::format("unrecognized variable name \"{}\"", statement.left->name);
    throw EvaluationError(what);
}

void Evaluator::visit(CallStmt& statement) {
    for (auto& scope : scopes | std::views::reverse) {
        auto it = scope.procedures.find(statement.target->name);
        if (it != scope.procedures.end()) {
            it->second.get().accept(*this);
            return;
        }
    }

    auto what = fmt::format("unrecognized procedure name \"{}\"", statement.target->name);
    throw EvaluationError(what);
}

void Evaluator::visit(ReadStmt& statement) {
    for (auto& scope : scopes | std::views::reverse) {
        auto it = scope.variables.find(statement.identifier->name);
        if (it != scope.variables.end()) {
            std::int64_t value;
            std::cout << "> " << std::flush;
            std::cin >> value;
            it->second = value;
            return;
        }
    }

    auto what = fmt::format("unrecognized variable name \"{}\"", statement.identifier->name);
    throw EvaluationError(what);
}

void Evaluator::visit(WriteStmt& statement) {
    statement.expression->accept(*this);
    fmt::println("{}", values.top());
}

void Evaluator::visit(BeginStmt& statement) {
    for (auto& child : statement.children) {
        child->accept(*this);
    }
}

void Evaluator::visit(IfStmt& statement) {
    statement.condition->accept(*this);

    if (condition_result) {
        statement.statement->accept(*this);
    }
}

void Evaluator::visit(WhileStmt& statement) {
    statement.condition->accept(*this);

    while (condition_result) {
        statement.statement->accept(*this);
        statement.condition->accept(*this);
    }
}

void Evaluator::visit(OddCond& condition) {
    condition.right->accept(*this);
    condition_result = values.top() % 2 == 1;
    values.pop();
}

void Evaluator::visit(EqualCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left == right;
}

void Evaluator::visit(NotEqualCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left != right;
}

void Evaluator::visit(LessThanCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left < right;
}

void Evaluator::visit(LessEqualCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left <= right;
}

void Evaluator::visit(GreaterThanCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left > right;
}

void Evaluator::visit(GreaterEqualCond& condition) {
    condition.left->accept(*this);
    condition.right->accept(*this);

    const auto right = values.top();
    values.pop();

    const auto left = values.top();
    values.pop();

    condition_result = left >= right;
}

void Evaluator::visit(NegationExpr& expression) {
    expression.right->accept(*this);
    values.top() = -values.top();
}

void Evaluator::visit(AdditionExpr& expression) {
    expression.left->accept(*this);
    expression.right->accept(*this);

    const auto right = values.top();
    values.pop();
    values.top() += right;
}

void Evaluator::visit(SubtractionExpr& expression) {
    expression.left->accept(*this);
    expression.right->accept(*this);

    const auto right = values.top();
    values.pop();
    values.top() -= right;
}

void Evaluator::visit(MultiplicationExpr& expression) {
    expression.left->accept(*this);
    expression.right->accept(*this);

    const auto right = values.top();
    values.pop();
    values.top() *= right;
}

void Evaluator::visit(DivisionExpr& expression) {
    expression.left->accept(*this);
    expression.right->accept(*this);

    const auto right = values.top();
    values.pop();
    values.top() /= right;
}

void Evaluator::visit(Number& number) {
    values.push(number.value);
}

void Evaluator::visit(Identifier& identifier) {
    for (const auto& scope : scopes | std::views::reverse) {
        const auto constant_it = scope.constants.find(identifier.name);
        if (constant_it != scope.constants.end()) {
            values.push(constant_it->second);
            return;
        }

        const auto variable_it = scope.variables.find(identifier.name);
        if (variable_it != scope.variables.end()) {
            values.push(variable_it->second);
            return;
        }
    }

    auto what = fmt::format("unrecognized symbol name \"{}\"", identifier.name);
    throw EvaluationError(what);
}
