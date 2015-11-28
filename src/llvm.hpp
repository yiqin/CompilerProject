
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <string>
#include "symbol.hpp"
#include <iostream>

namespace llvm {

// We want the register number is only used by one time.
// They are never the same.
static int register_number = 0;
static int get_register_number() {
  return register_number++;
};

static int label_number = 0;
static int get_label_number() {
  return label_number++;
};

// Something wrong with this case. 
// Fails to reset the code. 
// Create a headache to the unit test.
static void reset() {
  llvm::register_number = 0;
  llvm::label_number = 0;
};

static std::string end_of_line = ", align 4\n";



class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type type) 
          : type_(type), id_(std::string("R.")+std::to_string(get_register_number())) {}
           
    Register (const parser::Type type, const std::string id)
          : type_(type), id_(id) {}
      
    std::string type_llvm_ir () {
      if (type_ == parser::Type::INT) {
        return "i32";
      }
      return "/undefine type. Please wait./";
    }
    
    std::string name_llvm_ir () {
      return "%"+id_;
    }
    
    // <value>
    std::string value_llvm_ir() {
      return type_llvm_ir() + " " + name_llvm_ir();
    }
    
    // <pointer>
    std::string pointer_llvm_ir() {
      return type_llvm_ir() + "* " + name_llvm_ir();
    }
    
  private:
    const parser::Type type_;
    const std::string id_;
};

// Create new register
static Register::Ptr new_register(const parser::Type type) {
  Register::Ptr tmp = std::make_shared<Register>(type);
  return tmp;
};

static Register::Ptr new_register(const parser::Type type, const std::string id) {
  Register::Ptr tmp = std::make_shared<Register>(type, id);
  return tmp;
}

// Memory Access and Addressing Operations
// alloca <pointer>
static std::string alloca_instruction (parser::Symbol::Ptr symbol) {
  return std::string("%") + symbol->name() + " = alloca " + symbol->type_llvm_ir() + end_of_line;
}

// alloca <pointer>
static std::string alloca_instruction (Register::Ptr tmp) {
  return tmp->name_llvm_ir() + " = alloca " + tmp->type_llvm_ir() + end_of_line;
}

// Return value
// <value> = load <pointer>
static std::string load_instruction (llvm::Register::Ptr op_1, llvm::Register::Ptr op_2) {  
  std::string ir;
  ir += op_1->name_llvm_ir();
  ir += " = load ";
  ir += op_2->pointer_llvm_ir();
  ir += end_of_line;
  return ir;
}

// store <value>, <pointer>
static std::string store_instruction (llvm::Register::Ptr op_1, int integer_value) {
  std::string ir;
  ir += std::string("store i32 ") + std::to_string(integer_value);
  ir += ", ";
  ir += op_1->pointer_llvm_ir();
  ir += "\n";
  return ir;
}

// store <value>, <pointer>
static std::string store_instruction (llvm::Register::Ptr op_1, llvm::Register::Ptr op_2) {
  std::string ir;
  ir += std::string("store ") + op_1->value_llvm_ir();
  ir += ", ";
  ir += op_2->pointer_llvm_ir();
  ir += "\n";
  return ir;
}

  

}

// Binary Operations


// Other Operations


#endif  // __CSTR_COMPILER__LLVM_HPP