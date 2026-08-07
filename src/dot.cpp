#include "dot.hpp"

#include <fmt/format.h>

DotGenerator::DotGenerator()
    : id(0)
    , stack()
{}

void DotGenerator::visit(Program& program) {
    fmt::println("digraph G {{");
    fmt::println("  node [shape=oval];");

    start_unlinked_vertex("Program");
    program.block->accept(*this);
    end_vertex();

    fmt::println("}}");
}

void DotGenerator::visit(Block& block) {
    start_vertex("Block");

    if (!block.constants.empty()) {
        start_vertex("Constants");
        for (auto& constant : block.constants) {
            constant->accept(*this);
        }
        end_vertex();
    }

    if (!block.variables.empty()) {
        start_vertex("Variables");
        for (auto& variable : block.variables) {
            variable->accept(*this);
        }
        end_vertex();
    }

    if (!block.procedures.empty()) {
        start_vertex("Procedures");
        for (auto& procedure : block.procedures) {
            procedure->accept(*this);
        }
        end_vertex();
    }

    if (block.statement) {
        block.statement->accept(*this);
    }

    end_vertex(); // Block
}

void DotGenerator::visit(Constant& constant) {
    start_vertex("Constant");
    constant.identifier->accept(*this);
    constant.number->accept(*this);
    end_vertex();
}

void DotGenerator::visit(Variable& variable) {
    start_vertex("Variable");
    variable.identifier->accept(*this);
    end_vertex();
}

void DotGenerator::visit(Procedure& procedure) {
    start_vertex("Procedure");
    procedure.identifier->accept(*this);
    procedure.block->accept(*this);
    end_vertex();
}

void DotGenerator::visit(AssignmentStmt& statement) {
    start_vertex("AssignmentStmt");
    statement.left->accept(*this);
    statement.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(CallStmt& statement) {
    start_vertex("CallStmt");
    statement.target->accept(*this);
    end_vertex();
}

void DotGenerator::visit(ReadStmt& statement) {
    start_vertex("ReadStmt");
    statement.identifier->accept(*this);
    end_vertex();
}

void DotGenerator::visit(WriteStmt& statement) {
    start_vertex("WriteStmt");
    statement.expression->accept(*this);
    end_vertex();
}

void DotGenerator::visit(BeginStmt& statement) {
    start_vertex("BeginStmt");
    for (auto& child : statement.children) {
        child->accept(*this);
    }
    end_vertex();
}

void DotGenerator::visit(IfStmt& statement) {
    start_vertex("IfStmt");
    statement.condition->accept(*this);
    statement.statement->accept(*this);
    end_vertex();
}

void DotGenerator::visit(WhileStmt& statement) {
    start_vertex("WhileStmt");
    statement.condition->accept(*this);
    statement.statement->accept(*this);
    end_vertex();
}

void DotGenerator::visit(OddCond& condition) {
    start_vertex("OddCond");
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(EqualCond& condition) {
    start_vertex("EqualCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(NotEqualCond& condition) {
    start_vertex("NotEqualCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(LessThanCond& condition) {
    start_vertex("LessThanCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(LessEqualCond& condition) {
    start_vertex("LessEqualCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(GreaterThanCond& condition) {
    start_vertex("GreaterThanCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(GreaterEqualCond& condition) {
    start_vertex("GreaterEqualCond");
    condition.left->accept(*this);
    condition.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(NegationExpr& expression) {
    start_vertex("NegationExpr");
    expression.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(AdditionExpr& expression) {
    start_vertex("AdditionExpr");
    expression.left->accept(*this);
    expression.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(SubtractionExpr& expression) {
    start_vertex("SubtractionExpr");
    expression.left->accept(*this);
    expression.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(MultiplicationExpr& expression) {
    start_vertex("MultiplicationExpr");
    expression.left->accept(*this);
    expression.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(DivisionExpr& expression) {
    start_vertex("DivisionExpr");
    expression.left->accept(*this);
    expression.right->accept(*this);
    end_vertex();
}

void DotGenerator::visit(Number& number) {
    start_vertex("Number");

    fmt::println("  {} [label=\"{}\" shape=\"box\"]", id, number.value);
    stack.push_back(id++);
    add_edge_to_parent();
    end_vertex();

    end_vertex();
}

void DotGenerator::visit(Identifier& identifier) {
    start_vertex("Identifier");

    fmt::println("  {} [label=\"\\\"{}\\\"\" shape=\"box\"]", id, identifier.name);
    stack.push_back(id++);
    add_edge_to_parent();
    end_vertex();

    end_vertex();
}

void DotGenerator::start_vertex(const char *name) {
    start_unlinked_vertex(name);
    add_edge_to_parent();
}

void DotGenerator::start_unlinked_vertex(const char* name) {
    fmt::println("  {} [label=\"{}\"]", id, name);
    stack.push_back(id++);
}

void DotGenerator::end_vertex() {
    stack.pop_back();
}

void DotGenerator::add_edge_to_parent() {
    fmt::println("  {} -> {};", *(stack.end() - 2), *(stack.end() - 1));
}
