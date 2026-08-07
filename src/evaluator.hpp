#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "ast.hpp"
#include <exception>
#include <functional>
#include <map>
#include <stack>
#include <string_view>
#include <vector>
#include <cstdint>

class EvaluationError : public std::exception {
public:
    explicit EvaluationError(const char* what);
    explicit EvaluationError(const std::string& what);
};

class Evaluator final : public NodeVisitor {
public:
    Evaluator();
    void visit(Program& program) override;
    void visit(Block& block) override;
    void visit(Constant& constant) override;
    void visit(Variable& variable) override;
    void visit(Procedure& procedure) override;
    void visit(AssignmentStmt& statement) override;
    void visit(CallStmt& statement) override;
    void visit(ReadStmt& statement) override;
    void visit(WriteStmt& statement) override;
    void visit(BeginStmt& statement) override;
    void visit(IfStmt& statement) override;
    void visit(WhileStmt& statement) override;
    void visit(OddCond& condition) override;
    void visit(EqualCond& condition) override;
    void visit(NotEqualCond& condition) override;
    void visit(LessThanCond& condition) override;
    void visit(LessEqualCond& condition) override;
    void visit(GreaterThanCond& condition) override;
    void visit(GreaterEqualCond& condition) override;
    void visit(NegationExpr& expression) override;
    void visit(AdditionExpr& expression) override;
    void visit(SubtractionExpr& expression) override;
    void visit(MultiplicationExpr& expression) override;
    void visit(DivisionExpr& expression) override;
    void visit(Number& number) override;
    void visit(Identifier& identifier) override;

private:
    struct Scope {
        std::map<std::string_view, const std::int64_t> constants;
        std::map<std::string_view, int> variables;
        std::map<std::string_view, std::reference_wrapper<Procedure>> procedures;
    };

    std::vector<Scope> scopes;
    std::stack<std::int64_t> values;
    bool condition_result;
};

#endif // EVALUATOR_HPP
