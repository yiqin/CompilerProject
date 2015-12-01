
#include "llvm.hpp"

#include <iostream>
#include <string>

#include "ast.hpp"
#include "symbol.hpp"


namespace llvm {


std::string type (parser::Type t) {
    switch (t) {
        case parser::Type::INT:
            return "i32";
        case parser::Type::STRING:
            return "i8*";
    }
}


// The align 4 ensures that the address will be a multiple of 4
std::string alignment (parser::Type type) {
  switch (type) {
    case parser::Type::INT:
      return "align 4";
    case parser::Type::STRING:
      return "align 8";
  }
}


template <typename C, typename Op>
void infix (std::ostream& out, const char* delim, const C& container, Op op) {
    auto iter = std::begin(container);
    if (iter != std::end(container)) {
        out << op(*iter);
        for (++iter; iter != std::end(container); ++iter) {
            out << delim << op(*iter);
        }
    }
}


// void LLVM_Generator::visit (ast::Node::Ptr                   node) {}
// void LLVM_Generator::visit (ast::Expression::Ptr             node) {}
// void LLVM_Generator::visit (ast::Terminal::Ptr               node) {}
void LLVM_Generator::visit (ast::Declaration_List::Ptr       node) {
    for (auto& symbol : node->symbol_list()) {
        out_
            << '%' << symbol->name() << " = alloca " << type(symbol->type())
            << ", " << alignment(symbol->type())
            << std::endl
            ;
    }
}
void LLVM_Generator::visit (ast::Variable::Ptr               node) {
    const auto& symbol = node->symbol();
    
    std::string register_reference = '%' + symbol->name();
    register_reference += "." + to_string(increment_var_count_(symbol));
    
    register_reference_[node] = register_reference;

    switch (node->type()) {
        case parser::Type::INT:
            out_ << register_reference << " = load i32* %" << symbol->name()
                << std::endl;
            break;
        case parser::Type::STRING:
            // TODO
            break;
    }   
}
void LLVM_Generator::visit (ast::Const_Integer::Ptr          node) {
    register_reference_[node] = to_string(node->value());
}
void LLVM_Generator::visit (ast::Const_String::Ptr           node) {
    std::string id = "str." + to_string(const_strings_.size());
    const_strings_.emplace_back(id, node->value());

    register_reference_[node] = '%' + id;

    // TODO
    out_
        << '%' << id << " = getelementptr inbounds ["
        << node->value().size() + 1 << " x i8]* @." << id << ", i32 0, i32 0"
        << std::endl
        ;
    String::construct(node->value());
}
void LLVM_Generator::visit (ast::Unary_Expression::Ptr       node) {
    std::string register_ref = "%tmp." + to_string(register_reference_.size());
    register_reference_[node] = register_ref;

    node->rhs()->emit_code(*this);

    out_
        << register_ref << " = sub " << type(node->type()) << " 0, "
        << register_reference_[node->rhs()] << std::endl
        ;
}
void LLVM_Generator::visit (ast::Binary_Expression::Ptr      node) {
    std::string register_ref = "%tmp." + to_string(register_reference_.size());
    register_reference_[node] = register_ref;

    node->lhs()->emit_code(*this);
    node->rhs()->emit_code(*this);

    out_ << register_ref << " = ";

    switch (node->op()) {
        case ast::Operation::ADDITION:
            out_ << "add ";
            break;
        case ast::Operation::SUBTRACTION:
            out_ << "sub ";
            break;
        case ast::Operation::MULTIPLICATION:
            out_ << "mul ";
            break;
        case ast::Operation::DIVISION:
            out_ << "udiv ";
            break;
        case ast::Operation::MODULUS:
            out_ << "urem ";
            break;
        case ast::Operation::LEFT_SHIFT:
          // TODO
            out_ << "/Undefined. Please wait./ ";
            break;
        case ast::Operation::RIGHT_SHIFT:
          // TODO
            out_ << "/Undefined. Please wait./ ";
            break;
    }

    out_
        << type(node->type()) << ' '
        << register_reference_[node->lhs()] << ", "
        << register_reference_[node->rhs()] << std::endl
        ;
}
void LLVM_Generator::visit (ast::Condition::Ptr              node) {
    std::string register_ref = "%tmp." + to_string(register_reference_.size());
    register_reference_[node] = register_ref;

    node->lhs()->emit_code(*this);
    node->rhs()->emit_code(*this);

    out_ << register_ref << " = icmp ";

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

    switch (node->op()) {
        case ast::Comparison_Operation::EQUAL:
            out_ << "eq ";
            break;
        case ast::Comparison_Operation::NOT_EQUAL:
            out_ << "ne ";
            break;
        case ast::Comparison_Operation::LESS_THAN:
            out_ << "slt ";
            break;
        case ast::Comparison_Operation::GREATER_THAN:
            out_ << "sgt ";
            break;
        case ast::Comparison_Operation::LESS_THAN_OR_EQUAL:
            out_ << "sle ";
            break;
        case ast::Comparison_Operation::GREATER_THAN_OR_EQUAL:
            out_ << "sge ";
            break;
    }

    out_
        << type(node->type()) << ' '
        << register_reference_[node->lhs()] << ", "
        << register_reference_[node->rhs()] << std::endl
        ;
}
void LLVM_Generator::visit (ast::Assignment::Ptr             node) {
    const auto& symbol = node->lhs()->symbol();
    
    // alloca the symbol is needed.
    if (current_var_count_(symbol) == 0) {
        out_<< "%" << symbol->name() << " = alloca ";
        switch (symbol->type()) {
        case parser::Type::INT:
            out_<< "i32" << std::endl;
            break;
        case parser::Type::STRING:
            out_<< "i8*" << std::endl;
            break;
        }
        increment_var_count_(symbol);
    }
    
    std::string register_reference = '%' + symbol->name() + '.' +
        to_string(increment_var_count_(symbol));
    
    std::string register_ref = "%tmp." + to_string(register_reference_.size());
    register_reference_[node] = register_ref;

    node->rhs()->emit_code(*this);

    switch (node->type()) {
        case parser::Type::INT:
            out_
                << "store i32 " << register_reference_[node->rhs()]
                << ", i32* %" << node->lhs()->symbol()->name()
                << std::endl;
                ;
            break;
        case parser::Type::STRING:
            // TODO
            break;
    }
}
void LLVM_Generator::visit (ast::Function_Call::Ptr          node) {
    std::string register_ref = "%tmp." + to_string(register_reference_.size());
    register_reference_[node] = register_ref;

    auto& function  = node->function();
    auto& arguments = node->argument_list();

    // Step 1: Prepare arguments
    for (auto& argument : node->argument_list()) {
        argument->emit_code(*this);
    }

    // Step 2: call the function
    out_ << register_ref << " = call " << type(function->type()) << " (";
    infix(out_, ", ", function->argument_list(),
        [] (parser::Symbol::Ptr symbol) { return type(symbol->type()); });

    out_ << ")* @" << function->name() << "(";
    infix(out_, ", ", arguments,
        [&] (ast::Expression::Ptr expr) { 
            std::string tmp = type(expr->type()) + " " + register_reference_[expr];
            return tmp;// register_reference_[expr]; 
            });

    out_ << ')' << std::endl;
}
void LLVM_Generator::visit (ast::Instruction::Ptr            node) {
    // This is an empty instruction. Do nothing.
}
void LLVM_Generator::visit (ast::Expression_Instruction::Ptr node) {
    node->expression()->emit_code(*this);
}
void LLVM_Generator::visit (ast::Cond_Instruction::Ptr       node) {
    out_ << std::endl << "; Cond_Instruction" << std::endl << std::endl;

    llvm::Label label_0;
    llvm::Label label_1;
    llvm::Label label_2;

    // Step 1: condition
    node->condition()->emit_code(*this);
    out_ << llvm::br_instruction(register_reference_[node->condition()],
        label_0, label_1);

    // Step 2: instruction
    out_ << std::endl;
    out_ << label_0.destination_llvm_ir();
    node->instruction()->emit_code(*this);
    out_ << llvm::br_instruction(label_2);

    // Step 3: else_instruction
    out_ << std::endl;
    out_ << label_1.destination_llvm_ir();
    if (const auto& else_instruction = node->else_instruction()) {
        else_instruction->emit_code(*this);
    }
    out_ << llvm::br_instruction(label_2);

    // Step 4: the end
    out_ << std::endl;
    out_ << label_2.destination_llvm_ir();
}
void LLVM_Generator::visit (ast::While_Instruction::Ptr      node) {
    out_ << std::endl << "; While_Instruction" << std::endl << std::endl;

    llvm::Label label_0;
    llvm::Label label_1;
    llvm::Label label_2;

    out_ << llvm::br_instruction(label_0);

    // Step 1: condition
    out_ << std::endl;
    out_ << label_0.destination_llvm_ir();
    node->condition()->emit_code(*this);
    out_ << llvm::br_instruction(register_reference_[node->condition()],
        label_1, label_2);

    // Step 2: instruction
    out_ << std::endl;
    out_ << label_1.destination_llvm_ir();
    node->instruction()->emit_code(*this);
    out_ << llvm::br_instruction(label_0);

    // Step 3: the end
    out_ << std::endl;
    out_ << label_2.destination_llvm_ir();
}
void LLVM_Generator::visit (ast::Do_Instruction::Ptr         node) {
    out_ << std::endl << "; Do_Instruction" << std::endl << std::endl;

    llvm::Label label_0;
    llvm::Label label_1;
    llvm::Label label_2;

    out_ << llvm::br_instruction(label_0);

    // Step 1: instruction
    out_ << std::endl;
    out_ << label_0.destination_llvm_ir();
    node->instruction()->emit_code(*this);
    out_ << llvm::br_instruction(label_1);

    // Step 2: condition
    out_ << std::endl;
    out_ << label_1.destination_llvm_ir();
    node->condition()->emit_code(*this);
    out_ << llvm::br_instruction(register_reference_[node->condition()],
        label_0, label_2);

    // Step 3: the end
    out_ << std::endl;
    out_ << label_2.destination_llvm_ir();
}
void LLVM_Generator::visit (ast::For_Instruction::Ptr        node) {
    out_ << "\n; For_Instruction\n\n";

    llvm::Label label_0;
    llvm::Label label_1;
    llvm::Label label_2;
    llvm::Label label_3;

    // Step 1: initialization
    node->initialization()->emit_code(*this);
    out_ << llvm::br_instruction(label_0);

    // Step 2: condition
    out_ << std::endl;
    out_ << label_0.destination_llvm_ir();
    node->condition()->emit_code(*this);
    out_ << llvm::br_instruction(register_reference_[node->condition()],
        label_1, label_3);

    // Step 3: instruction, the body of the for instruction
    out_ << std::endl;
    out_ << label_1.destination_llvm_ir();
    node->instruction()->emit_code(*this);
    out_ << llvm::br_instruction(label_2);

    // Step 4: increment
    out_ << std::endl;
    out_ << label_2.destination_llvm_ir();
    node->increment()->emit_code(*this);
    out_ << llvm::br_instruction(label_0);

    // Step 5: the end
    out_ << std::endl;
    out_ << label_3.destination_llvm_ir();
}
void LLVM_Generator::visit (ast::Return_Instruction::Ptr     node) {
    node->expression()->emit_code(*this);

    out_ << "ret ";
    out_ << type(node->expression()->type()) << ' ';
    out_ << register_reference_[node->expression()];
    out_ << std::endl;
}
void LLVM_Generator::visit (ast::Compound_Instruction::Ptr   node) {
    for (auto& instruction : node->instruction_list()) {
        instruction->emit_code(*this);
    }
}
void LLVM_Generator::visit (ast::Function_Declaration::Ptr   node) {
    auto& declarator = node->function_declarator();

    // step 1
    out_ << "declare ";

    // step 2: function return type
    out_ << type(node->type());

    // step 3: function name
    out_ << " @" << declarator->name();

    // step 4: function argument list
    out_ << "(";
    infix(out_, ", ", declarator->argument_list(),
        [] (parser::Symbol::Ptr symbol) { return type(symbol->type()); });
    out_ << ")";

    // step 5
    out_ << std::endl;
}
void LLVM_Generator::visit (ast::Function_Definition::Ptr    node) {
    auto& declarator = node->function_declarator();

    // step 1
    out_ << "define ";

    // step 2: function return type
    out_ << type(node->type());

    // step 3: function name
    out_ << " @" << declarator->name();

    // step 4: function argument list
    out_ << "(";
    infix(out_, ", ", declarator->argument_list(),
        [] (parser::Symbol::Ptr symbol) {
            return type(symbol->type()) + " %" + symbol->name();
        });

    out_ << ")";

    // step 5
    out_ << " {" << std::endl << "entry:" << std::endl;
    
    // step 6 alloca argument variables
    for (auto& symbol : declarator->argument_list()) {        
        std::string tmp_type;
        switch (symbol->type()) {
        case parser::Type::INT:
            tmp_type = "i32";
            break;
        case parser::Type::STRING:
            tmp_type = "i8*";
            break;
        }
                
        out_<< "%" << symbol->name() << ".pointer" << " = alloca " << tmp_type << std::endl;

        out_<< "store " << tmp_type << " %" << symbol->name() << ", ";
        out_<< tmp_type << "* %" << symbol->name() << ".pointer" << std::endl;    
        
        // Simply change the name to .pointer
        symbol->name(symbol->name() + ".pointer");        
        increment_var_count_(symbol);
    }

    // step 7: function body
    node->body()->emit_code(*this);
    out_ << "}" << std::endl;
}


ID_Factory Label::id_factory_;
// ID_Factory Register::id_factory_;
ID_Factory String::id_factory_;

std::vector<String::Ptr> String::all_strings_;


// br instruction
// br label <dest>
std::string br_instruction (const Label& label_1) {
  return std::string("br ") + label_1.name_llvm_ir() + '\n';
};

// br i1 <cond>, label <iftrue>, label<iffailure>
std::string br_instruction (const std::string& cond, const Label& label_1, const Label& label_2) {
   return "br i1 " + cond + ", " + label_1.name_llvm_ir() + ", " + label_2.name_llvm_ir() + '\n';
};

// <value> = getelementptr (class string)
std::string getelementptr_instruction (llvm::String::Ptr string) {
};

}  // namespace llvm
