#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

struct Node;
struct Program;
struct Block;
struct Constant;
struct Variable;
struct Procedure;

struct Stmt;
struct AssignmentStmt;
struct CallStmt;
struct ReadStmt;
struct WriteStmt;
struct BeginStmt;
struct IfStmt;
struct WhileStmt;

struct Cond;
struct UnaryCond;
struct BinaryCond;
struct OddCond;
struct EqualCond;
struct NotEqualCond;
struct LessThanCond;
struct LessEqualCond;
struct GreaterThanCond;
struct GreaterEqualCond;

struct Expr;
struct UnaryExpr;
struct BinaryExpr;
struct NegationExpr;
struct AdditionExpr;
struct SubtractionExpr;
struct MultiplicationExpr;
struct DivisionExpr;

struct Number;
struct Identifier;

struct NodeVisitor {
    virtual ~NodeVisitor() = default;
    virtual void visit(Program& program) = 0;
    virtual void visit(Block& block) = 0;
    virtual void visit(Constant& constant) = 0;
    virtual void visit(Variable& variable) = 0;
    virtual void visit(Procedure& procedure) = 0;
    virtual void visit(AssignmentStmt& statement) = 0;
    virtual void visit(CallStmt& statement) = 0;
    virtual void visit(ReadStmt& statement) = 0;
    virtual void visit(WriteStmt& statement) = 0;
    virtual void visit(BeginStmt& statement) = 0;
    virtual void visit(IfStmt& statement) = 0;
    virtual void visit(WhileStmt& statement) = 0;
    virtual void visit(OddCond& condition) = 0;
    virtual void visit(EqualCond& condition) = 0;
    virtual void visit(NotEqualCond& condition) = 0;
    virtual void visit(LessThanCond& condition) = 0;
    virtual void visit(LessEqualCond& condition) = 0;
    virtual void visit(GreaterThanCond& condition) = 0;
    virtual void visit(GreaterEqualCond& condition) = 0;
    virtual void visit(NegationExpr& expression) = 0;
    virtual void visit(AdditionExpr& expression) = 0;
    virtual void visit(SubtractionExpr& expression) = 0;
    virtual void visit(MultiplicationExpr& expression) = 0;
    virtual void visit(DivisionExpr& expression) = 0;
    virtual void visit(Number& number) = 0;
    virtual void visit(Identifier& identifier) = 0;
};

struct Node {
    virtual ~Node() = default;
    virtual void accept(NodeVisitor& visitor) = 0;
};

struct Program final : public Node {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Block> block;
};

struct Block final : public Node {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::vector<std::unique_ptr<Constant>> constants;
    std::vector<std::unique_ptr<Variable>> variables;
    std::vector<std::unique_ptr<Procedure>> procedures;
    std::unique_ptr<Stmt> statement;
};

struct Constant final : public Node {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<Number> number;
};

struct Variable final : public Node {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> identifier;
};

struct Procedure final : public Node {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<Block> block;
};

struct Stmt : public Node {
    virtual ~Stmt() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;
};

struct AssignmentStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> left;
    std::unique_ptr<Expr> right;
};

struct CallStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> target;
};

struct ReadStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Identifier> identifier;
};

struct WriteStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Expr> expression;
};

struct BeginStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::vector<std::unique_ptr<Stmt>> children;
};

struct IfStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Cond> condition;
    std::unique_ptr<Stmt> statement;
};

struct WhileStmt final : public Stmt {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Cond> condition;
    std::unique_ptr<Stmt> statement;
};

struct Cond : public Node {
    virtual ~Cond() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;
};

struct UnaryCond : public Cond {
    virtual ~UnaryCond() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;

    std::unique_ptr<Expr> right;
};

struct BinaryCond : public Cond {
    virtual ~BinaryCond() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;

    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct OddCond final : public UnaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct EqualCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct NotEqualCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct LessThanCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct LessEqualCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct GreaterThanCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct GreaterEqualCond final : public BinaryCond {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct Expr : public Node {
    virtual ~Expr() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;
};

struct UnaryExpr : public Expr {
    virtual ~UnaryExpr() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;

    std::unique_ptr<Expr> right;
};

struct BinaryExpr : public Expr {
    virtual ~BinaryExpr() override = default;
    virtual void accept(NodeVisitor& visitor) override = 0;

    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct NegationExpr final : public UnaryExpr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct AdditionExpr final : public BinaryExpr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct SubtractionExpr final : public BinaryExpr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct MultiplicationExpr final : public BinaryExpr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct DivisionExpr final : public BinaryExpr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct Number final : public Expr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::int64_t value;
};

struct Identifier final : public Expr {
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string name;
};

#endif // AST_HPP
