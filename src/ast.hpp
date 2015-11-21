
#include <memory>
#include <string>
#include "symbol.hpp"


namespace ast {

struct Operation {
  /* data */
};

class Node {
  public:
    typedef std::shared_ptr<Node> Ptr;

    // virtual std::string emit_llvm_ir ();
    // virtual void build_llvm_ir ();
};


class Terminal : public Node {
};


class Variable : public Terminal {
  public:
    parser::Symbol::Ptr symbol;
};


template <typename T>
class Const : public Terminal {
  public:
    T value;
};


class Expression : public Node {};


class Unary_Expression : public Expression {
  public:
    Operation op;
    Expression::Ptr expression;
};

// Binary_Expression - Expression class for a binary operator
class Binary_Expression : public Expression {
  public:
    Operation op;
    Expression::Ptr left;
    Expression::Ptr right;

    std::string build_llvm_ir () {
        return "";
    }

  private:
    std::string temp_name_;
};


class Assignment : public Expression {
  public:
    Variable::Ptr variable;
    Expression::Ptr expression;
};


class Function_Call : public Node {
  public:
    std::vector<Expression::Ptr> argument_list;
};


class Instruction : public Node {
  public:

};


class Expression_Instruction : public Instruction {};


// class Iteration_Instruction :


class While_Instruction : public Instruction {
  public:
    Expression::Ptr condition;
    Instruction::Ptr instruction;
};


class Do_Instruction : public Instruction {
  public:
    Expression::Ptr condition;
    Instruction::Ptr instruction;
};


class For_Instruction : public Instruction {
  public:
    Assignment::Ptr initialization;
    Expression::Ptr condition;
    Assignment::Ptr increment;
    Instruction::Ptr instruction;
};


class Return_Instruction : public Instruction {
  public:
    Expression::Ptr expression;
};


// Block
class Compound_Instruction : public Instruction {
  public:
    std::vector<Instruction::Ptr> instruction_list;
};


}  // namespace ast
