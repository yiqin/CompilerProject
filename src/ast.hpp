
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <memory>
#include <string>
#include "symbol.hpp"


namespace ast {


enum class Operation {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    MODULUS,
    LEFT_SHIFT,
    RIGHT_SHIFT,

    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
};


class Node {
  public:
    typedef std::shared_ptr<Node> Ptr;

    virtual ~Node () {}

    // virtual std::string emit_llvm_ir ();
    
    // We don't need this.
    // virtual void build_llvm_ir ();
};

// e.g. 0, 0+1, 1-2, a, a+b, a-b, a+(c+d)
class Expression : public Node {
  public:
    typedef std::shared_ptr<Expression> Ptr;

    Expression (const parser::Type& type) : type_(type) {}

    const parser::Type& type () const { return type_; }
    
    virtual std::string emit_llvm_ir () {
      // Assume all integers are %32
      return std::string("undefine Expression");
    }

  private:
    const parser::Type& type_;
};

// Yi: What is Terminal? I always forget...
class Terminal : public Expression {
  public:
    typedef std::shared_ptr<Terminal> Ptr;

    Terminal (const parser::Type& type) : Expression(type) {}
};

// Declaration, external_declaration 
// It corresponds to Symbol class and Function class

// For a single declarator, may not be used in the future.
class Symbol_Declarator : public Node {
  public:
    typedef std::shared_ptr<Symbol_Declarator> Ptr;
    
    Symbol_Declarator (const parser::Symbol::Ptr& symbol)
          : symbol_(symbol) {}
    
    std::string emit_llvm_ir () {
      if (symbol_->type() == parser::Type::INT) {
        return std::string("%") + symbol_->name() + " = alloca i32, align 4";
      } else {
        return "undefined symbol with string type";
      }
    }
    
  private:
    parser::Symbol::Ptr symbol_;
};

// Declarate a list of symbols
class Declaration : public Node {
  public:
    typedef std::shared_ptr<Declaration> Ptr;
    
    Declaration (const parser::Symbol_List& symbol_list)
          : symbol_list_(symbol_list) {}
    
    std::string emit_llvm_ir () {
      std::string ir;
      
      for (auto& symbol : symbol_list_) { 
        Symbol_Declarator::Ptr symbol_declarator = std::make_shared<Symbol_Declarator>(symbol);
        ir += symbol_declarator->emit_llvm_ir()+"\n";
      }
      // remove the last \n
      if (ir.size() > 0) {
        ir.pop_back();
      }
      
      return ir;
    }
    
  private:
    parser::Symbol_List symbol_list_;
};

// e.g. int main(int a)
// FIXME: I hesitate to use Function class or function_definition.
// But I think function_definition is better. 
class Function_Definition : public Node {
  public:
    typedef std::shared_ptr<Function_Definition> Ptr;
    
    Function_Definition (const parser::Type& type, parser::Function::Ptr& function_declarator)
          : type_(type), function_declarator_(function_declarator) {}
    
    std::string emit_llvm_ir () {
      std::string ir;
      
      // step 1
      ir += "define";
      
      // step 2: function return type
      if (type_ == parser::Type::INT) {
        ir += " i32";
      }
      
      // step 3: function name
      ir += " @"+function_declarator_->name();
      
      // step 4: function argument list
      ir += "(";
     
      for (auto& symbol : function_declarator_->argument_list()) {
        if(symbol->type() == parser::Type::INT) {
          ir += "i32 %" + symbol->name();
          ir += ", ";
        }
      }
      // remove the last space and the last comma
      if (function_declarator_->argument_list().size() > 0) {
        ir.pop_back();
        ir.pop_back();
      }
        
      ir += ")";
      
      // step 5
      ir += " #0";
      
      return ir;
    }
    
  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
    // decl_glb_fct
};

// Todo: with function, we need a block_start and a block_end
// Maybe not.

class Variable : public Terminal {
  public:
    typedef std::shared_ptr<Variable> Ptr;

    Variable (const parser::Symbol::Ptr& symbol)
          : Terminal(symbol->type()), symbol_(symbol) {}
          
    std::string emit_llvm_ir () {
      if (symbol_->type() == parser::Type::INT) {
        return "i32* %" + symbol_->name();
      } else {
        return "undefined symbol with string type";
      }
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

    std::string emit_llvm_ir () {
      // Assume all integers are %32
      return std::string("i32 ") + std::to_string(value_);
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

  private:
    std::string value_;
};


class Unary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Unary_Expression> Ptr;

    Unary_Expression (const parser::Type& type, Operation op, Expression::Ptr rhs)
          : Expression(type), op_(op), rhs_(rhs) {}

  private:
    Operation op_;  // <Operation> is a placeholder type.
    Expression::Ptr rhs_;
};


// TODO: Yi - this is complicated, and come back later.
// Binary_Expression - Expression class for a binary operator
class Binary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Binary_Expression> Ptr;

    Binary_Expression (const parser::Type& type,
        Operation op, Expression::Ptr lhs, Expression::Ptr rhs)
          : Expression(type), op_(op), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
        return "hello world";
    }

  private:
    Operation op_;  // <Operation> is a placeholder type.
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;

    std::string temp_name_;
};


// For example: a = 1;
class Assignment : public Expression {
  public:
    typedef std::shared_ptr<Assignment> Ptr;

    // Why is type an argument in the constructor? 
    Assignment (const parser::Type& type,
        Variable::Ptr lhs, Expression::Ptr rhs)
          : Expression(type), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
      std::string ir = std::string("store ") + rhs_->emit_llvm_ir()+ ", " + lhs_->emit_llvm_ir() + ", align 4";
      return ir;
    }

  private:
    Variable::Ptr lhs_;
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

  private:
    parser::Function::Ptr function_;
    std::vector<Expression::Ptr> argument_list_;
};


// What is an instruction?
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


class Conditional_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Conditional_Instruction> Ptr;

    Conditional_Instruction (
        Expression::Ptr condition,
        Instruction::Ptr instruction
    )
          : condition_(condition),
            instruction_(instruction) {}

    Conditional_Instruction (
        Expression::Ptr condition,
        Instruction::Ptr instruction,
        Instruction::Ptr else_instruction
    )
          : condition_(condition),
            instruction_(instruction),
            else_instruction_(else_instruction) {}

  private:
    Expression::Ptr condition_;
    Instruction::Ptr instruction_;
    Instruction::Ptr else_instruction_;
};


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

// jump_instruction in the parser
// RETURN expression ';'
// i.e. return 1;
// i.e. return a;
class Return_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Return_Instruction> Ptr;

    Return_Instruction (Expression::Ptr expression)
          : expression_(expression) {}
          
    std::string emit_llvm_ir () {
      std::string ir;
      
      // dynamic pointer cast
      // const_integer
      auto const_integer = std::dynamic_pointer_cast<ast::Const_Integer>(expression_);
      if (const_integer) {
        ir += "ret "+ const_integer->emit_llvm_ir();
        return ir;
      }
      
      // variable
      auto variable = std::dynamic_pointer_cast<ast::Variable>(expression_);
      if (variable) {
        // TODO: we need a register tracker.
        std::string next_register = std::string("%3");
        // create a new register, this could be a symbol
        
        
        ir += next_register + " = load "+ variable->emit_llvm_ir()+", align 4";
        ir += "\n";
        ir += "ret ";
        
        // TODO: user the emit_llvm_ir()
        ir += "i32 %3";
        
        return ir;
      }
      
      ir += "Undefined yet. Please wait.";
      
      return ir;
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

  private:
    std::vector<Instruction::Ptr> instruction_list_;
};


}  // namespace ast


#endif  // __CSTR_COMPILER__AST_HPP
