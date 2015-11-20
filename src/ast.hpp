
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <memory>

#include "symbol.hpp"


namespace ast {


class Node {
  public:
    typedef std::shared_ptr<Node> Ptr;

    virtual ~Node () {}

    // virtual std::string emit_llvm_ir ();
    // virtual void build_llvm_ir ();
};


class Terminal : public Node {
  public:
    typedef std::shared_ptr<Terminal> Ptr;
};


class Variable : public Terminal {
  public:
    typedef std::shared_ptr<Variable> Ptr;

    Variable (const parser::Symbol::Ptr& symbol) : symbol_(symbol) {}

  private:
    parser::Symbol::Ptr symbol_;
};


template <typename T>
class Const : public Terminal {
  public:
    typedef std::shared_ptr<Const> Ptr;

    Const (const T& value) : value_(value) {}

  private:
    T value_;
};


class Expression : public Node {
  public:
    typedef std::shared_ptr<Expression> Ptr;
};


class Unary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Unary_Expression> Ptr;

    Unary_Expression (int op, Expression::Ptr rhs)
          : op_(op), rhs_(rhs) {}

  private:
    int op_;  // <int> is a placeholder type.
    Expression::Ptr rhs_;
};


class Binary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Binary_Expression> Ptr;

    Binary_Expression (int op, Expression::Ptr lhs, Expression::Ptr rhs)
          : op_(op), lhs_(lhs), rhs_(rhs) {}

    // llvm::Value* build_llvm_ir (llvm::IRBuilder<>& builder) {
    //     llvm::Value* lhs = left->build_llvm_ir(builder);
    //     llvm::Value* rhs = right->build_llvm_ir(builder);
    //     return builder.CreateBinOp(llvm::Instruction::Mul, left, right, temp_name_);
    // }

  private:
    int op_;  // <int> is a placeholder type.
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;

    std::string temp_name_;
};


class Assignment : public Expression {
  public:
    typedef std::shared_ptr<Assignment> Ptr;

    Assignment (Variable::Ptr lhs, Expression::Ptr rhs)
          : lhs_(lhs), rhs_(rhs) {}

  private:
    Variable::Ptr lhs_;
    Expression::Ptr rhs_;
};


class Function_Call : public Expression {
  public:
    typedef std::shared_ptr<Function_Call> Ptr;

    Function_Call (
        parser::Function::Ptr function,
        const std::vector<Expression::Ptr>& argument_list
    )
          : function_(function),
            argument_list_(argument_list) {}

  private:
    parser::Function::Ptr function_;
    std::vector<Expression::Ptr> argument_list_;
};


class Instruction : public Node {
  public:
    typedef std::shared_ptr<Instruction> Ptr;
};


class Expression_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Expression_Instruction> Ptr;

    Expression_Instruction (Expression::Ptr expression)
          : expression_(expression) {}

  private:
    Expression::Ptr expression_;
};


// class Iteration_Instruction :


class While_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<While_Instruction> Ptr;

    While_Instruction (Expression::Ptr condition, Instruction::Ptr instruction)
          : condition_(condition), instruction_(instruction) {}

  private:
    Expression::Ptr condition_;
    Instruction::Ptr instruction_;
};


class Do_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Do_Instruction> Ptr;

    Do_Instruction (Expression::Ptr condition, Instruction::Ptr instruction)
          : condition_(condition), instruction_(instruction) {}

  private:
    Expression::Ptr condition_;
    Instruction::Ptr instruction_;
};


class For_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<For_Instruction> Ptr;

    For_Instruction (
        Assignment::Ptr initialization,
        Expression::Ptr condition,
        Assignment::Ptr increment,
        Instruction::Ptr instruction
    )
          : initialization_(initialization),
            condition_(condition),
            increment_(increment),
            instruction_(instruction) {}

  private:
    Assignment::Ptr initialization_;
    Expression::Ptr condition_;
    Assignment::Ptr increment_;
    Instruction::Ptr instruction_;
};


class Return_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Return_Instruction> Ptr;

    Return_Instruction (Expression::Ptr expression)
          : expression_(expression) {}

  private:
    Expression::Ptr expression_;
};


// Block
class Compound_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Compound_Instruction> Ptr;

    Compound_Instruction (const std::vector<Instruction::Ptr>& instruction_list)
          : instruction_list_(instruction_list) {}

  private:
    std::vector<Instruction::Ptr> instruction_list_;
};


}  // namespace ast


#endif  // __CSTR_COMPILER__AST_HPP
