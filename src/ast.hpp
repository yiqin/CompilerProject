
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <memory>
#include <string>
#include "symbol.hpp"
#include "llvm.hpp"

#include <iostream>

namespace ast {

static std::string end_of_line = ", align 4\n";

// Operation relates to Abstract Syntax Tree.
// Operation and Comparison Operation are in different mechanisms.
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


class Node {
  public:
    typedef std::shared_ptr<Node> Ptr;

    virtual ~Node () {}

    virtual std::string emit_llvm_ir () {
      return std::string("; /undefine Expression - Node Class/ \n");
    };
    
    // We don't need this.
    // virtual void build_llvm_ir ();
    
};

// e.g. 0, 0+1, 1-2, a, a+b, a-b, a+(c+d)
class Expression : public Node {
  public:
    typedef std::shared_ptr<Expression> Ptr;

    // Yi: I remove (const parser::Type& type) to (const parser::Type type)
    // If you don't agree, please let me know.
    // (const parser::Type& type) doesn't save the type information.
    Expression (const parser::Type type) 
          : type_(type) {
            result_register_ = std::make_shared<llvm::Pointer_Register>(type_);
          }

    const parser::Type type () const { return type_; }
    llvm::Pointer_Register::Ptr result_register () { return result_register_; }
    
    void update_result_register (llvm::Pointer_Register::Ptr new_register) {
      result_register_ = new_register;
    }
    
    std::string emit_llvm_ir () {
      return std::string("/undefine Expression - Expression Class/");
    }

  private:
    const parser::Type type_;
    llvm::Pointer_Register::Ptr result_register_;
};

// Yi: What is Terminal? I always forget...
class Terminal : public Expression {
  public:
    typedef std::shared_ptr<Terminal> Ptr;

    Terminal (const parser::Type type) : Expression(type) {}
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
        return llvm::alloca_instruction(symbol_);
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
        ir += symbol_declarator->emit_llvm_ir();
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
          : Terminal(symbol->type()), symbol_(symbol) {
      update_result_register(llvm::new_pointer_register(parser::Type::INT, symbol_->name()));
    }
          
    // FIXME: this is wrong.
    // This should return empty string.
    std::string emit_llvm_ir () {
      return "";
      // return "/undefined symbol with string type/";
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
      // %1 = alloca i32, align 4
      // store i32 0, i32* %1

      std::string ir = llvm::alloca_instruction(result_register());
      ir += llvm::store_instruction(value_, result_register());
      
      return ir;
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


// Binary_Expression - Expression class for a binary operator
// e.g. i <= 10, 1+2
class Binary_Expression : public Expression {
  public:
    typedef std::shared_ptr<Binary_Expression> Ptr;

    Binary_Expression (const parser::Type& type,
        Operation op, Expression::Ptr lhs, Expression::Ptr rhs)
          : Expression(type), op_(op), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
      // Actually we have an abstract syntax tree 
      // Not simply left and right, then operation.
      // No. We can simple left and right.
      
      std::string ir;
      
      // Step 1: lhs_ emit_llvm_ir
      // Get the lhs data into one register
      
      llvm::Value_Register::Ptr value_register_lhs = llvm::new_value_register(parser::Type::INT);
      
      ir += lhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_lhs, lhs_->result_register());

      // Step 2: rhs_ emit_llvm_ir
      // Get the rhs data into another register
      llvm::Value_Register::Ptr value_register_rhs = llvm::new_value_register(parser::Type::INT);
      
      ir += rhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_rhs, rhs_->result_register());
      
      // FIXME: Fails to put this part of code to llvm.hpp
      
      // Step 3: Operation
      llvm::Value_Register::Ptr tmp_value_register = llvm::new_value_register(parser::Type::INT);

      ir += tmp_value_register->name_llvm_ir();
      ir += " = ";
      
      // add
      // sub 
      // mul
      // udiv
      // urem
      switch (op_) {
        case Operation::ADDITION:
          ir += "add";
          break;
        case Operation::SUBTRACTION:
          ir += "sub";
          break;
        case Operation::MULTIPLICATION:
          ir += "mul";
          break;
        case Operation::DIVISION:
          ir += "udiv";
          break;
        case Operation::MODULUS:
          ir += "urem";
          break;
       case Operation::LEFT_SHIFT:
          ir += "/Undefine. Please wait./";
          break;
       case Operation::RIGHT_SHIFT:
          ir += "/Undefine. Please wait./";
          break;
      }
      
      ir += " ";
      ir += value_register_lhs->type_llvm_ir();
      ir += " ";
      ir += value_register_lhs->name_llvm_ir();
      ir += ", ";
      ir += value_register_rhs->name_llvm_ir();
      
      ir += "\n";
      ir += llvm::alloca_instruction(result_register());
      ir += llvm::store_instruction(tmp_value_register, result_register());
      
      return ir;
    }

  private:
    Operation op_;  // <Operation> is a placeholder type.
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;

    std::string temp_name_;
};

// TODO: Do we merge Binary_Expression and Condition?
// Yi: One expression can have many +-*/(), but can only have one >=, ==
// So we can seperate to two different class.
// But they are quite similar.
// Currently it works, we can merge them in the future.
// Return are different
class Condition : public Expression {
  public:
    typedef std::shared_ptr<Condition> Ptr;
  
    Condition (Expression::Ptr lhs, Comparison_Operation comparison_operator, 
                   Expression::Ptr rhs) 
          : Expression(parser::Type::INT), lhs_(lhs), comparison_operator_(comparison_operator), rhs_(rhs) {
            result_register_ = std::make_shared<llvm::Value_Register>(parser::Type::INT);            
          }
    
    // override
    llvm::Value_Register::Ptr result_register() { return result_register_; }
    
    std::string emit_llvm_ir () {
      std::string ir;
      
      // Step 1: lhs_ emit_llvm_ir
      // Get the lhs data into one register
      
      llvm::Value_Register::Ptr value_register_lhs = llvm::new_value_register(parser::Type::INT);
      
      ir += lhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_lhs, lhs_->result_register());

      // Step 2: rhs_ emit_llvm_ir
      // Get the rhs data into another register
      llvm::Value_Register::Ptr value_register_rhs = llvm::new_value_register(parser::Type::INT);
      
      ir += rhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_rhs, rhs_->result_register());
      
      // Step 3: Compare
      ir += result_register()->name_llvm_ir();
      ir += " = icmp ";
      
      // eq: equal
      // ne: not equal
      // ugt: unsigned greater than
      // uge: unsigned greater or equal
      // ult: unsigned less than
      // ule: unsigned less or equal
      // sgt: signed greater than
      // sge: signed greater or equal
      // slt: signed less than
      // sle: signed less or equal

      switch (comparison_operator_) {
        case Comparison_Operation::EQUAL:
          ir += "eq";
          break;
        case Comparison_Operation::NOT_EQUAL:
          ir += "ne";
          break;
        case Comparison_Operation::LESS_THAN:
          ir += "slt";
          break;
        case Comparison_Operation::GREATER_THAN:
          ir += "sgt";
          break;
        case Comparison_Operation::LESS_THAN_OR_EQUAL:
          ir += "sle";
          break;
       case Comparison_Operation::GREATER_THAN_OR_EQUAL:
          ir += "sge";
          break;
      }
      
      ir += " ";
      ir += value_register_lhs->type_llvm_ir();
      ir += " " + value_register_lhs->name_llvm_ir() + ", " + value_register_rhs->name_llvm_ir();
      ir += "\n";
      return ir;
    }
  
  
  private:
    Expression::Ptr lhs_;
    Comparison_Operation comparison_operator_;
    Expression::Ptr rhs_;
    // override
    llvm::Value_Register::Ptr result_register_;
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
      
      std::string ir;
      
      // Step 1: load the expression data in to a value

      llvm::Value_Register::Ptr tmp_value_register = llvm::new_value_register(parser::Type::INT);
      
      ir += rhs_->emit_llvm_ir();
      ir += llvm::load_instruction(tmp_value_register, rhs_->result_register());
      
      // Step 2: assignment the value to the variable
      ir += llvm::store_instruction(tmp_value_register, lhs_->result_register());
      
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


// Doesn't have a result register.
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

// If else ??
// select_instruction
// cond_instruction 
//
class Cond_Instruction : public Instruction {
  public:
    typedef std::shared_ptr<Cond_Instruction> Ptr;

    Cond_Instruction (
        Expression::Ptr condition,
        Instruction::Ptr instruction
    )
          : condition_(condition),
            instruction_(instruction) {}

    Cond_Instruction (
        Expression::Ptr condition,
        Instruction::Ptr instruction,
        Instruction::Ptr else_instruction
    )
          : condition_(condition),
            instruction_(instruction),
            else_instruction_(else_instruction) {}
            
    std::string emit_llvm_ir () {
      std::string ir;
      
      return ir;
    }

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
        Condition::Ptr condition,
        Assignment::Ptr increment,
        Instruction::Ptr instruction
    )
          : initialization_(initialization),
            condition_(condition),
            increment_(increment),
            instruction_(instruction) {}
            
    std::string emit_llvm_ir () {
      std::string ir;
      ir += "\n; For_Instruction\n\n";
      
      llvm::Label::Ptr label_1 = llvm::new_label();
      llvm::Label::Ptr label_2 = llvm::new_label();
      llvm::Label::Ptr label_3 = llvm::new_label();
      llvm::Label::Ptr label_4 = llvm::new_label();
      
      // Step 1: initialization
      ir += initialization_->emit_llvm_ir();
      ir += llvm::br_instruction(label_1);
      
      // Step 2: condition
      ir += "\n";
      ir += label_1->destination_llvm_ir();
      ir += condition_->emit_llvm_ir();
      ir += llvm::br_instruction(condition_->result_register(), label_2, label_4);
      
      // Step 3: instruction, the body of the for instruction
      ir += "\n";
      ir += label_2->destination_llvm_ir();
      ir += instruction_->emit_llvm_ir();
      ir += llvm::br_instruction(label_3);
      
      // Step 4: increment
      ir += "\n";
      ir += label_3->destination_llvm_ir();
      ir += increment_->emit_llvm_ir();
      ir += llvm::br_instruction(label_1);
      
      // Step 5: the end
      ir += "\n";
      ir += label_4->destination_llvm_ir();
      
      return ir;
    }

  private:
    Assignment::Ptr initialization_;
    Condition::Ptr condition_;
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
      
      // Step 1: load the expression data into the register
      
      // this register is only used in the emit_llvm_ir. It's private.
      // int tmp_register_number = llvm::get_register_number();
      llvm::Value_Register::Ptr tmp_value_register = llvm::new_value_register(parser::Type::INT);
      
      ir += expression_->emit_llvm_ir();
      
      ir += llvm::load_instruction(tmp_value_register, expression_->result_register());
      
      // Step 2: return the register
      ir += "ret " + tmp_value_register->value_llvm_ir();
      ir += "\n";
      
      /*
      // dynamic pointer cast
      // const_integer
      auto const_integer = std::dynamic_pointer_cast<ast::Const_Integer>(expression_);
      if (const_integer) {
        ir += "ret "+ const_integer->inline_llvm_ir();
        return ir;
      }
      
      // variable
      auto variable = std::dynamic_pointer_cast<ast::Variable>(expression_);
      if (variable) {
        // TODO: we need a register tracker.
        std::string next_register = std::string("%3");
        // create a new register, this could be a symbol
        
        
        ir += next_register + " = load "+ variable->emit_llvm_ir()+end_of_line;
        ir += "ret ";
        
        // TODO: user the emit_llvm_ir()
        ir += "i32 %3";
        
        return ir;
      }
      
      ir += "Undefined yet. Please wait.";
      */
      
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
