
#ifndef __CSTR_COMPILER__AST_HPP
#define __CSTR_COMPILER__AST_HPP


#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "llvm.hpp"
#include "symbol.hpp"


namespace ast {

const std::string end_of_line = ", align 4\n";

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
};

// e.g. 0, 0+1, 1-2, a, a+b, a-b, a+(c+d)
class Expression : public Node {
  public:
    typedef std::shared_ptr<Expression> Ptr;

    Expression (const parser::Type& type)
          : type_(type) {}

    const parser::Type& type () const { return type_; }

    std::string emit_llvm_ir () {
        return std::string("/undefine Expression - Expression Class/");
    }

    virtual std::string emit_llvm_ir_access () = 0;

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


class Variable : public Terminal {
  public:
    typedef std::shared_ptr<Variable> Ptr;

    Variable (const parser::Symbol::Ptr& symbol)
          : Terminal(symbol->type()), symbol_(symbol) {
        symbol_->increment_access_count();
        register_reference_ = '%' + symbol_->name() + '.' +
            to_string(symbol_->access_count());
    }

    std::string emit_llvm_ir () {
        switch (type()) {
            case parser::Type::INT:
                return  register_reference_ + " = load i32* %" +
                    symbol_->name() + '\n';
            case parser::Type::STRING:
                // TODO
                return "";
        }
    }

    std::string emit_llvm_ir_access () {
        // TODO(Emery): Access global variables.
        return register_reference_;
    }

    std::string llvm_pointer_register () {
        return '%' + symbol_->name();
    }

  private:
    std::string register_reference_;
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
        return "";
    }

    std::string emit_llvm_ir_access () {
        return to_string(value_);
    }

  private:
    int value_;
};

// For example: "hello world"
class Const_String : public Terminal {
  public:
    typedef std::shared_ptr<Const_String> Ptr;

    Const_String (const std::string& value)
          : Terminal(parser::Type::STRING) {
        string_ = llvm::String::construct(value);
    }

    const std::string& id () const { return string_->id(); }

    std::string emit_llvm_ir () {
        std::string ir;
        ir += "%" + string_->id() + " = ";
        ir += llvm::getelementptr_instruction(string_);
        return ir;
    }

    std::string emit_llvm_ir_access () {
        return string_->id();
    }

  private:
    llvm::String::Ptr string_;
};


class Nonterminal : public Expression {
  public:
    Nonterminal (const parser::Type& type)
          : Expression(type),
            id_("tmp." + id_factory_.get_id()) {}

    const std::string& id () const { return id_; }

    std::string emit_llvm_ir_access () {
        return '%' + id_;
    }

  private:
    static llvm::ID_Factory id_factory_;

    const std::string id_;
};


class Unary_Expression : public Nonterminal {
  public:
    typedef std::shared_ptr<Unary_Expression> Ptr;

    // TODO: Yi - Remove if we don't use this constructor.
    Unary_Expression (const parser::Type& type, Operation op, Expression::Ptr rhs)
          : Nonterminal(type), op_(op), rhs_(rhs) {}

    Unary_Expression (Expression::Ptr rhs)
          : Nonterminal(parser::Type::INT), op_(Operation::SUBTRACTION), rhs_(rhs) {}

    std::string emit_llvm_ir () {
      std::string ir;

      ir += rhs_->emit_llvm_ir();

      ir += '%' + id();
      ir += " = ";
      ir += "sub";
      ir += " ";
      ir += llvm::type(type());
      ir += " ";
      ir += "0";
      ir += ", ";
      ir += rhs_->emit_llvm_ir_access();
      ir += "\n";

      return ir;
    }

  private:
    Operation op_;
    Expression::Ptr rhs_;
};


// Binary_Expression - Expression class for a binary operator
// e.g. i <= 10, 1+2
class Binary_Expression : public Nonterminal {
  public:
    typedef std::shared_ptr<Binary_Expression> Ptr;

    Binary_Expression (const parser::Type& type,
        Operation op, Expression::Ptr lhs, Expression::Ptr rhs)
          : Nonterminal(type), op_(op), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
      std::string ir;

      ir += lhs_->emit_llvm_ir();
      ir += rhs_->emit_llvm_ir();

      ir += '%' + id();
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
          // TODO
          ir += "/Undefine. Please wait./";
          break;
        case Operation::RIGHT_SHIFT:
          // TODO
          ir += "/Undefine. Please wait./";
          break;
      }

      ir += " ";
      ir += llvm::type(type());
      ir += " ";
      ir += lhs_->emit_llvm_ir_access();
      ir += ", ";
      ir += rhs_->emit_llvm_ir_access();

      ir += "\n";

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
class Condition : public Nonterminal {
  public:
    typedef std::shared_ptr<Condition> Ptr;

    Condition (Expression::Ptr lhs, Comparison_Operation comparison_operator,
                   Expression::Ptr rhs)
          : Nonterminal(parser::Type::INT),
            lhs_(lhs),
            comparison_operator_(comparison_operator),
            rhs_(rhs) {}

    std::string emit_llvm_ir () {
      std::string ir;

      ir += lhs_->emit_llvm_ir();
      ir += rhs_->emit_llvm_ir();

      ir += '%' + id();
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
      ir += ' ' + llvm::type(type()) + ' ';

      ir += lhs_->emit_llvm_ir_access();
      ir += ", ";
      ir += rhs_->emit_llvm_ir_access();
      return ir;
    }


  private:
    Expression::Ptr lhs_;
    Comparison_Operation comparison_operator_;
    Expression::Ptr rhs_;
};

// For example: a = 1; b = "hello world";
class Assignment : public Nonterminal {
  public:
    typedef std::shared_ptr<Assignment> Ptr;

    Assignment (Variable::Ptr lhs, Expression::Ptr rhs)
          : Nonterminal(lhs->type()), lhs_(lhs), rhs_(rhs) {}

    std::string emit_llvm_ir () {
        std::string ir;
        ir += rhs_->emit_llvm_ir();

        switch (type()) {
            case parser::Type::INT:
                ir += "store i32 " + rhs_->emit_llvm_ir_access() +
                    ", i32* " + lhs_->llvm_pointer_register() + '\n';
                break;
            case parser::Type::STRING:
                // TODO
                break;
        }

        return ir;
    }

    std::string emit_llvm_ir_access () {
        return lhs_->emit_llvm_ir_access();
    }

  private:
    Variable::Ptr lhs_;
    Expression::Ptr rhs_;
};


class Function_Call : public Nonterminal {
  public:
    typedef std::shared_ptr<Function_Call> Ptr;

    Function_Call (parser::Function::Ptr function)
          : Nonterminal(function->type()),
            function_(function) {}

    Function_Call (
        parser::Function::Ptr function,
        const std::vector<Expression::Ptr>& argument_list
    )
          : Nonterminal(function->type()),
            function_(function),
            argument_list_(argument_list) {}

    std::string emit_llvm_ir () {
        std::string ir;

        // Step 1: Prepare arguments
        for (auto& argument : argument_list_) {
            ir += argument->emit_llvm_ir();
        }

        // Step 2: call the function
        std::ostringstream oss;
        oss << '%' << id() << " = call " << function_->type_llvm_ir() << " (";
        std::transform(
            std::begin(function_->argument_list()),
            std::end(function_->argument_list()),
            std::ostream_iterator<std::string>(oss, ", "),
            [] (parser::Symbol::Ptr symbol) { return symbol->type_llvm_ir(); }
        );
        oss << " @" << function_->name() << " (";
        std::transform(
            std::begin(argument_list_), std::end(argument_list_),
            std::ostream_iterator<std::string>(oss, ", "),
            [] (Expression::Ptr expr) { return expr->emit_llvm_ir_access(); }
        );
        oss << ")\n";

        return ir;
    }

  private:
    parser::Function::Ptr function_;
    std::vector<Expression::Ptr> argument_list_;
};


// Doesn't have a result register.
class Instruction : public Node {
  public:
    typedef std::shared_ptr<Instruction> Ptr;

    // Empty instruction.
    std::string emit_llvm_ir () { return ""; }
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
      ir += llvm::br_instruction(condition_->emit_llvm_ir_access(),
        label_0, label_1);

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
      ir += llvm::br_instruction(condition_->emit_llvm_ir_access(),
        label_1, label_2);

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
      ir += llvm::br_instruction(condition_->emit_llvm_ir_access(), label_0, label_2);

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
      ir += llvm::br_instruction(condition_->emit_llvm_ir_access(), label_1, label_3);

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

      ir += expression_->emit_llvm_ir();

      ir += "ret ";
      ir += llvm::type(expression_->type()) + ' ';
      ir += expression_->emit_llvm_ir_access();
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

    std::string emit_llvm_ir () {
      std::string ir;
      for (auto& instruction : instruction_list_) {
        ir += instruction->emit_llvm_ir();
        ir += "\n";
      }
      return ir;
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

    std::string emit_llvm_ir () {
      std::string ir;

      // step 1
      ir += "declare ";

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

      for (auto& symbol : function_declarator_->argument_list()) {
        ir += symbol->type_llvm_ir() + ", ";
      }

      // remove the last space and the last comma
      if (function_declarator_->argument_list().size() > 0) {
        ir.pop_back();
        ir.pop_back();
      }

      ir += ")";

      // step 5
      // // FIXME: this may not be always 0
      // ir += " #0";
      ir += "\n";

      return ir;
    }

  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
    // decl_glb_fct
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
      ir += " {\nentry:\n";

      // step 6: function body
      ir += body_->emit_llvm_ir();
      ir += "}\n";

      return ir;
    }

  private:
    parser::Type type_;
    parser::Function::Ptr function_declarator_;
    Compound_Instruction::Ptr body_;
    // decl_glb_fct
};




}  // namespace ast


#endif  // __CSTR_COMPILER__AST_HPP
