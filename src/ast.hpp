
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "symbol.hpp"


namespace ast {


class Node;
class Expression;
class Terminal;
class Declaration_List;
class Variable;
class Const_Integer;
class Const_String;
class Unary_Expression;
class Binary_Expression;
class Condition;
class Assignment;
class Function_Call;
class Instruction;
class Expression_Instruction;
class Cond_Instruction;
class While_Instruction;
class Do_Instruction;
class For_Instruction;
class Return_Instruction;
class Compound_Instruction;
class Function_Declaration;
class Function_Definition;


// Visitor base class for code generation.
class Code_Generator {
  public:
    virtual void visit (std::shared_ptr<Node> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Node'.");
    }
    virtual void visit (std::shared_ptr<Expression> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Expression'.");
    }
    virtual void visit (std::shared_ptr<Terminal> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Terminal'.");
    }
    virtual void visit (std::shared_ptr<Declaration_List> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Declaration'.");
    }
    virtual void visit (std::shared_ptr<Variable> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Variable'.");
    }
    virtual void visit (std::shared_ptr<Const_Integer> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Const_Integer'.");
    }
    virtual void visit (std::shared_ptr<Const_String> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Const_String'.");
    }
    virtual void visit (std::shared_ptr<Unary_Expression> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Unary_Expression'.");
    }
    virtual void visit (std::shared_ptr<Binary_Expression> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Binary_Expression'.");
    }
    virtual void visit (std::shared_ptr<Condition> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Condition'.");
    }
    virtual void visit (std::shared_ptr<Assignment> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Assignment'.");
    }
    virtual void visit (std::shared_ptr<Function_Call> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Function_Call'.");
    }
    virtual void visit (std::shared_ptr<Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Instruction'.");
    }
    virtual void visit (std::shared_ptr<Expression_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Expression_Instruction'.");
    }
    virtual void visit (std::shared_ptr<Cond_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Cond_Instruction'.");
    }
    virtual void visit (std::shared_ptr<While_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'While_Instruction'.");
    }
    virtual void visit (std::shared_ptr<Do_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Do_Instruction'.");
    }
    virtual void visit (std::shared_ptr<For_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'For_Instruction'.");
    }
    virtual void visit (std::shared_ptr<Return_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Return_Instruction'.");
    }
    virtual void visit (std::shared_ptr<Compound_Instruction> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Compound_Instruction'.");
    }
    virtual void visit (std::shared_ptr<Function_Declaration> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Function_Declaration'.");
    }
    virtual void visit (std::shared_ptr<Function_Definition> node) {
        throw std::runtime_error("This code generator does not implement a handler for node class 'Function_Definition'.");
    }
};

enum class Operation {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    MODULUS,
    LEFT_SHIFT,
    RIGHT_SHIFT,
};

enum class Comparison_Operation {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
};


class Node : public std::enable_shared_from_this<Node> {
  public:
    typedef std::shared_ptr<Node> Ptr;

    virtual ~Node () {}

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  protected:
    template <typename T>
    std::shared_ptr<T> self (T*) {
        return std::static_pointer_cast<T>(shared_from_this());
    }

    template <typename T>
    const std::shared_ptr<T const> self (T const*) const {
        return std::static_pointer_cast<T const>(shared_from_this());
    }
};

// e.g. 0, 0+1, 1-2, a, a+b, a-b, a+(c+d)
class Expression : public Node {
  public:
    typedef std::shared_ptr<Expression> Ptr;

    Expression (const parser::Type& type)
          : type_(type) {}

    const parser::Type& type () const { return type_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    const parser::Type type_;
};


class Terminal : public Expression {
  public:
    typedef std::shared_ptr<Terminal> Ptr;

    Terminal (const parser::Type type) : Expression(type) {}
};

// Declaration, external_declaration
// It corresponds to Symbol class and Function class

// // For a single declarator, may not be used in the future.
// class Symbol_Declarator : public Node {
//   public:
//     typedef std::shared_ptr<Symbol_Declarator> Ptr;

//     Symbol_Declarator (const parser::Symbol::Ptr& symbol)
//           : symbol_(symbol) {}

//     virtual void emit_code (Code_Generator& generator) {
//->        generator.visit(self(this));
//     }

//     const parser::Symbol::Ptr& symbol () const { return symbol_; }

//   private:
//     parser::Symbol::Ptr symbol_;
// };



class Variable : public Terminal {
  public:
    typedef std::shared_ptr<Variable> Ptr;

    Variable (const parser::Symbol::Ptr& symbol)
          : Terminal(symbol->type()), symbol_(symbol) {}

    const parser::Symbol::Ptr& symbol () const { return symbol_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    parser::Symbol::Ptr symbol_;
};

// e.g. 1, 2, 3, ...
// For example: 1, 2, 3, ...
class Const_Integer : public Terminal {
  public:
    typedef std::shared_ptr<Const_Integer> Ptr;

    Const_Integer (const int& value)
          : Terminal(parser::Type::INT), value_(value) {}

    int value () const { return value_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    int value_;
};

// For example: "hello world"
class Const_String : public Terminal {
  public:
    typedef std::shared_ptr<Const_String> Ptr;

    Const_String (const std::string& value)
          : Terminal(parser::Type::STRING), value_(value) {}

    const std::string& value () const { return value_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    std::string value_;
};


class Unary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Unary_Expression> Ptr;

    Unary_Expression (Expression::Ptr rhs)
          : Expression(parser::Type::INT), op_(Operation::SUBTRACTION), rhs_(rhs) {}

    const Operation&       op  () const { return op_;  }
    const Expression::Ptr& rhs () const { return rhs_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Operation op_;
    Expression::Ptr rhs_;
};


// Binary_Expression - Expression class for a binary operator
// e.g. i <= 10, 1+2
class Binary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Binary_Expression> Ptr;

    Binary_Expression (const parser::Type& type,
        Operation op, Expression::Ptr lhs, Expression::Ptr rhs)
          : Expression(type), op_(op), lhs_(lhs), rhs_(rhs) {}

    const Operation&       op  () const { return op_;  }
    const Expression::Ptr& lhs () const { return lhs_; }
    const Expression::Ptr& rhs () const { return rhs_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Operation op_;
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;
};

// return type is i1
class Condition : public Expression {
  public:
    typedef std::shared_ptr<Condition> Ptr;

    Condition (Comparison_Operation op, Expression::Ptr lhs,
               Expression::Ptr rhs)
          : Expression(lhs->type()),
            op_(op),
            lhs_(lhs),
            rhs_(rhs) {}

    const Comparison_Operation& op  () const { return op_;  }
    const Expression::Ptr&      lhs () const { return lhs_; }
    const Expression::Ptr&      rhs () const { return rhs_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Comparison_Operation op_;
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;
};

// For example: a = 1; b = "hello world";
class Assignment : public Expression {
  public:
    typedef std::shared_ptr<Assignment> Ptr;

    Assignment (Variable::Ptr lhs, Expression::Ptr rhs)
          : Expression(lhs->type()), lhs_(lhs), rhs_(rhs) {}

    const Variable::Ptr&   lhs () const { return lhs_; }
    const Expression::Ptr& rhs () const { return rhs_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Variable::Ptr   lhs_;
    Expression::Ptr rhs_;
};


class Function_Call : public Expression {
  public:
    typedef std::shared_ptr<Function_Call> Ptr;

    Function_Call (parser::Function::Ptr function)
          : Expression(function->type()),
            function_(function) {}

    Function_Call (
        parser::Function::Ptr function,
        const std::vector<Expression::Ptr>& argument_list
    )
          : Expression(function->type()),
            function_(function),
            argument_list_(argument_list) {}

    const parser::Function::Ptr&        function      () const { return function_;      }
    const std::vector<Expression::Ptr>& argument_list () const { return argument_list_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    parser::Function::Ptr function_;
    std::vector<Expression::Ptr> argument_list_;
};


class Instruction : public Node {
  public:
    typedef std::shared_ptr<Instruction> Ptr;

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }
};


class Expression_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Expression_Instruction> Ptr;

    Expression_Instruction (Expression::Ptr expression)
          : expression_(expression) {}

    const Expression::Ptr& expression () const { return expression_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Expression::Ptr expression_;
};

// Declarate a list of symbols
class Declaration_List : public Instruction {
  public:
    typedef std::shared_ptr<Declaration_List> Ptr;

    Declaration_List () {}

    Declaration_List (parser::Symbol_List symbol_list)
          : symbol_list_(symbol_list) {}

    const parser::Symbol_List& symbol_list () const { return symbol_list_; }

    void push_back (parser::Symbol::Ptr symbol) { symbol_list_.push_back(symbol); }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    parser::Symbol_List symbol_list_;
};


// If else, control flow
class Cond_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Cond_Instruction> Ptr;

    Cond_Instruction (
        Condition::Ptr condition,
        Instruction::Ptr instruction
    )
          : condition_(condition),
            instruction_(instruction) {}

    Cond_Instruction (
        Condition::Ptr condition,
        Instruction::Ptr instruction,
        Instruction::Ptr else_instruction
    )
          : condition_(condition),
            instruction_(instruction),
            else_instruction_(else_instruction) {}

    const Condition::Ptr&   condition        () const { return condition_;        }
    const Instruction::Ptr& instruction      () const { return instruction_;      }
    const Instruction::Ptr& else_instruction () const { return else_instruction_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Condition::Ptr condition_;
    Instruction::Ptr instruction_;
    Instruction::Ptr else_instruction_;
};


class While_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<While_Instruction> Ptr;

    While_Instruction (Condition::Ptr condition, Instruction::Ptr instruction)
          : condition_(condition), instruction_(instruction) {}

    const Condition::Ptr&   condition   () const { return condition_;   }
    const Instruction::Ptr& instruction () const { return instruction_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Condition::Ptr condition_;
    Instruction::Ptr instruction_;
};


class Do_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Do_Instruction> Ptr;

    Do_Instruction (Condition::Ptr condition, Instruction::Ptr instruction)
          : condition_(condition), instruction_(instruction) {}

    const Condition::Ptr&   condition   () const { return condition_;   }
    const Instruction::Ptr& instruction () const { return instruction_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Condition::Ptr condition_;
    Instruction::Ptr instruction_;
};


class For_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<For_Instruction> Ptr;

    For_Instruction (
        Expression::Ptr initialization,
        Condition::Ptr condition,
        Expression::Ptr increment,
        Instruction::Ptr instruction
    )
          : initialization_(initialization),
            condition_(condition),
            increment_(increment),
            instruction_(instruction) {}

    const Expression::Ptr&  initialization () const { return initialization_; }
    const Condition::Ptr&   condition      () const { return condition_;      }
    const Expression::Ptr&  increment      () const { return increment_;      }
    const Instruction::Ptr& instruction    () const { return instruction_;    }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Expression::Ptr initialization_;
    Condition::Ptr condition_;
    Expression::Ptr increment_;
    Instruction::Ptr instruction_;
};


// jump_instruction in the parser
// RETURN expression ';'
// i.e. return 1;
// i.e. return a;
class Return_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Return_Instruction> Ptr;

    Return_Instruction (Expression::Ptr expression)
          : expression_(expression) {}

    const Expression::Ptr& expression () const { return expression_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    Expression::Ptr expression_;
};

// For example
// {
//    // a lot of code.....
// }
class Compound_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Compound_Instruction> Ptr;

    Compound_Instruction (const std::vector<Instruction::Ptr>& instruction_list)
          : instruction_list_(instruction_list) {}

    Compound_Instruction (std::vector<Instruction::Ptr>&& instruction_list)
          : instruction_list_(std::move(instruction_list)) {}

    const std::vector<Instruction::Ptr>& instruction_list () const { return instruction_list_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    std::vector<Instruction::Ptr> instruction_list_;
};


// e.g. int main(int a);
class Function_Declaration : public Node {
  public:
    typedef std::shared_ptr<Function_Declaration> Ptr;

    Function_Declaration (const parser::Type& type, parser::Function::Ptr& function_declarator)
          : type_(type), function_declarator_(function_declarator) {}

    const parser::Type&          type                () const { return type_;                }
    const parser::Function::Ptr& function_declarator () const { return function_declarator_; }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
};


// e.g. int main(int a) { ... }
class Function_Definition : public Node {
  public:
    typedef std::shared_ptr<Function_Definition> Ptr;

    Function_Definition (
        const parser::Type& type,
        parser::Function::Ptr function_declarator,
        Compound_Instruction::Ptr body
    )
          : type_(type),
            function_declarator_(function_declarator),
            body_(body) {}

    const parser::Type&              type                () const { return type_;                }
    const parser::Function::Ptr&     function_declarator () const { return function_declarator_; }
    const Compound_Instruction::Ptr& body                () const { return body_;                }

    virtual void emit_code (Code_Generator& generator) {
        generator.visit(self(this));
    }

  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
    Compound_Instruction::Ptr body_;
};

}  // namespace ast


#endif  // __CSTR_COMPILER__AST_HPP
