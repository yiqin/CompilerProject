
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <iostream>
#include <memory>
#include <string>

#include "llvm.hpp"
#include "symbol.hpp"


namespace ast {

const std::string end_of_line = ", align 4\n";

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

    // TODO (Emery) Make abstract.
    virtual std::string emit_llvm_ir () {
        return std::string("; /undefine Expression - Node Class/ \n");
    };
    // virtual std::string emit_llvm_ir () = 0;

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
          : result_register_(std::make_shared<llvm::Pointer_Register>(type)) {}

    const parser::Type& type                    () const { return result_register_->type(); }
    llvm::Pointer_Register::Ptr result_register ()       { return result_register_;         }

    void update_result_register (llvm::Pointer_Register::Ptr new_register) {
        result_register_ = new_register;
    }

    std::string emit_llvm_ir () {
      return std::string("/undefine Expression - Expression Class/");
    }

  private:
    // FIXME: Chnage to Register
    // Argumnets in llvm ir instructions must be pointer_register, or value_register
    // If it's changed to Register, check several errors llvm.hpp file. 
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
      return llvm::alloca_instruction(symbol_);
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
      ir += "define ";

      // step 2: function return type
      switch (type_) {
        case parser::Type::INT:
          ir += "i32";
          break;
        case parser::Type::STRING:
          ir += "i8*";
          break;
      }

      // step 3: function name
      ir += " @"+function_declarator_->name();

      // step 4: function argument list
      ir += "(";

      // TODO: argument is <value>. We need to put them into <pointer>
      for (auto& symbol : function_declarator_->argument_list()) {
        ir += symbol->type_llvm_ir() + " %" + symbol->name() + ", ";
      }
      
      // remove the last space and the last comma
      if (function_declarator_->argument_list().size() > 0) {
        ir.pop_back();
        ir.pop_back();
      }

      ir += ")";

      // step 5
      // FIXME: this may not be always 0
      ir += " #0";
      ir += "\n";

      return ir;
    }

  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
    // decl_glb_fct
};


class Variable : public Terminal {
  public:
    typedef std::shared_ptr<Variable> Ptr;

    Variable (const parser::Symbol::Ptr& symbol)
          : Terminal(symbol->type()), symbol_(symbol) {
      update_result_register(std::make_shared<llvm::Pointer_Register>(symbol_->type(), symbol_->name()));
    }

    // Return empty string. 
    // result_register is %<symbol_->name()>
    std::string emit_llvm_ir () {
      return "";
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
          : Terminal(parser::Type::STRING), string_(std::make_shared<llvm::String>(std::move(value))) {
      id_ = string_->id();
    }
    
    std::string emit_llvm_ir () {
      std::string ir;
      
      ir += llvm::alloca_instruction(result_register());
      ir += llvm::store_instruction(string_, result_register());
      
      return ir;
    }
    
    std::string declare_llvm_ir () {
      // declarated outside any scope.
      // @.str = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
      std::string ir;
      
      ir += id_ + " = private unnamed_addr constant ";
      ir += "[" + std::to_string(string_->value().size()+1) + std::string(" x i8] ");
      ir += "c\"" + string_->value() + "\\00\"";
      ir += ", align 1";
      ir += "\n";
      
      return ir;
    }

  private:
    std::string id_;
    llvm::String::Ptr string_;
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

      llvm::Value_Register::Ptr value_register_lhs = std::make_shared<llvm::Value_Register>(lhs_->type());

      ir += lhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_lhs, lhs_->result_register());

      // Step 2: rhs_ emit_llvm_ir
      // Get the rhs data into another register
      llvm::Value_Register::Ptr value_register_rhs = std::make_shared<llvm::Value_Register>(rhs_->type());

      ir += rhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_rhs, rhs_->result_register());

      // FIXME: Fails to put this part of code to llvm.hpp

      // Step 3: Operation
      llvm::Value_Register::Ptr tmp_value_register = std::make_shared<llvm::Value_Register>(result_register()->type());

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
      ir += result_register()->type_llvm_ir();
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
    Operation op_;
    Expression::Ptr lhs_;
    Expression::Ptr rhs_;
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
          : Expression(parser::Type::INT),
            lhs_(lhs),
            comparison_operator_(comparison_operator),
            rhs_(rhs) {
        result_register_ = std::make_shared<llvm::Value_Register>(parser::Type::INT);
    }

    // override
    llvm::Value_Register::Ptr result_register() { return result_register_; }

    std::string emit_llvm_ir () {
      std::string ir;

      // Step 1: lhs_ emit_llvm_ir
      // Get the lhs data into one register

      llvm::Value_Register::Ptr value_register_lhs = std::make_shared<llvm::Value_Register>(lhs_->type());

      ir += lhs_->emit_llvm_ir();
      ir += llvm::load_instruction(value_register_lhs, lhs_->result_register());

      // Step 2: rhs_ emit_llvm_ir
      // Get the rhs data into another register
      llvm::Value_Register::Ptr value_register_rhs = std::make_shared<llvm::Value_Register>(rhs_->type());

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

    Assignment (Variable::Ptr lhs, Expression::Ptr rhs)
          : Expression(lhs->type()), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
      std::string ir;

      // Step 1: load the expression data in to a value
      llvm::Value_Register::Ptr tmp_value_register = std::make_shared<llvm::Value_Register>(rhs_->type());

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
    
    std::string emit_llvm_ir () {
      return expression_->emit_llvm_ir();
    }

  private:
    Expression::Ptr expression_;
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

    std::string emit_llvm_ir () {
      std::string ir;
      
      ir += "\n; Cond_Instruction\n\n";
      
      llvm::Label::Ptr label_0 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_1 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_2 = std::make_shared<llvm::Label>();
      
      // Step 1: condition
      ir += condition_->emit_llvm_ir();
      ir += llvm::br_instruction(condition_->result_register(), label_0, label_1);

      // Step 2: instruction
      ir += "\n";
      ir += label_0->destination_llvm_ir();
      ir += instruction_->emit_llvm_ir();
      ir += llvm::br_instruction(label_2);

      // Step 3: else_instruction
      ir += "\n";
      ir += label_1->destination_llvm_ir();
      auto tmp_instruction = else_instruction_;
      if (tmp_instruction) {
        ir += tmp_instruction->emit_llvm_ir();
      }
      ir += llvm::br_instruction(label_2);

      // Step 4: the end
      ir += "\n";
      ir += label_2->destination_llvm_ir();
      
      return ir;
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
    
    std::string emit_llvm_ir () {
      std::string ir;
      ir += "\n; While_Instruction\n\n";
      
      llvm::Label::Ptr label_0 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_1 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_2 = std::make_shared<llvm::Label>();
      
      ir += llvm::br_instruction(label_0);
      
      // Step 1: condition
      ir += "\n";
      ir += label_0->destination_llvm_ir();
      ir += condition_->emit_llvm_ir();
      ir += llvm::br_instruction(condition_->result_register(), label_1, label_2);

      // Step 2: instruction
      ir += "\n";
      ir += label_1->destination_llvm_ir();
      ir += instruction_->emit_llvm_ir();
      ir += llvm::br_instruction(label_0);

      // Step 3: the end
      ir += "\n";
      ir += label_2->destination_llvm_ir();
      
      return ir;
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
          
    std::string emit_llvm_ir () {
      std::string ir;
      ir += "\n; Do_Instruction\n\n";
      
      llvm::Label::Ptr label_0 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_1 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_2 = std::make_shared<llvm::Label>();
      
      ir += llvm::br_instruction(label_0);
      
      // Step 1: instruction
      ir += "\n";
      ir += label_0->destination_llvm_ir();
      ir += instruction_->emit_llvm_ir();
      ir += llvm::br_instruction(label_1);
      
      // Step 2: condition
      ir += "\n";
      ir += label_1->destination_llvm_ir();
      ir += condition_->emit_llvm_ir();
      ir += llvm::br_instruction(condition_->result_register(), label_0, label_2);

      // Step 3: the end
      ir += "\n";
      ir += label_2->destination_llvm_ir();
      
      return ir;
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

    std::string emit_llvm_ir () {
      std::string ir;
      ir += "\n; For_Instruction\n\n";

      llvm::Label::Ptr label_0 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_1 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_2 = std::make_shared<llvm::Label>();
      llvm::Label::Ptr label_3 = std::make_shared<llvm::Label>();

      // Step 1: initialization
      ir += initialization_->emit_llvm_ir();
      ir += llvm::br_instruction(label_0);

      // Step 2: condition
      ir += "\n";
      ir += label_0->destination_llvm_ir();
      ir += condition_->emit_llvm_ir();
      ir += llvm::br_instruction(condition_->result_register(), label_1, label_3);

      // Step 3: instruction, the body of the for instruction
      ir += "\n";
      ir += label_1->destination_llvm_ir();
      ir += instruction_->emit_llvm_ir();
      ir += llvm::br_instruction(label_2);

      // Step 4: increment
      ir += "\n";
      ir += label_2->destination_llvm_ir();
      ir += increment_->emit_llvm_ir();
      ir += llvm::br_instruction(label_0);

      // Step 5: the end
      ir += "\n";
      ir += label_3->destination_llvm_ir();

      return ir;
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

    std::string emit_llvm_ir () {
      std::string ir;

      // Step 1: load the expression data into a value register
      llvm::Value_Register::Ptr tmp_value_register = std::make_shared<llvm::Value_Register>(expression_->type());

      ir += expression_->emit_llvm_ir();

      ir += llvm::load_instruction(tmp_value_register, expression_->result_register());

      // Step 2: return the register
      ir += "ret " + tmp_value_register->value_llvm_ir();
      ir += "\n";

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
