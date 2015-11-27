
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

static void reset() {
  register_number = 0;
  label_number = 0;
};

static std::string end_of_line = ", align 4\n";

class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type type) 
          : type_(type), id_(std::to_string(get_register_number())) {}
           
    Register (const parser::Type type, const std::string id)
          : type_(type), id_(id) {}
      
    std::string type_ir () {
      if (type_ == parser::Type::INT) {
        return "i32";
      }
      return "/undefine type. Please wait./";
    }
    
    std::string name_llvm_ir () {
      return "%"+id_;
    }
    
    std::string value_llvm_ir() {
      return type_ir() + " " + name_llvm_ir();
    }
    
    std::string pointer_llvm_ir() {
      return type_ir() + "* " + name_llvm_ir();
    }
    
  private:
    const parser::Type type_;
    const std::string id_;
};

static Register::Ptr new_register(const parser::Type type) {
  Register::Ptr tmp = std::make_shared<Register>(type);
  return tmp;
};

static Register::Ptr new_register(const parser::Type type, const std::string id) {
  Register::Ptr tmp = std::make_shared<Register>(type, id);
  return tmp;
}

// Memory Access and Addressing Operations
static std::string alloca_instruction (parser::Symbol::Ptr symbol) {
  return std::string("%") + symbol->name() + " = alloca " + symbol->type_ir() + end_of_line;
}

static std::string alloca_instruction (Register::Ptr tmp) {
  return tmp->name_llvm_ir() + " = alloca " + tmp->type_ir() + end_of_line;
}

static std::string load_instruction (llvm::Register::Ptr lhs, llvm::Register::Ptr rhs) {  
  std::string ir;
  ir += lhs->name_llvm_ir();
  ir += " = load ";
  ir += rhs->pointer_llvm_ir();
  ir += end_of_line;
  return ir;
}

// 
static std::string store_instruction (llvm::Register::Ptr lhs, int integer_value) {
  std::string ir;
  ir += std::string("store i32 ") + std::to_string(integer_value);
  ir += ", ";
  ir += lhs->pointer_llvm_ir();
  ir += "\n";
  return ir;
}

  

}

// Binary Operations


// Other Operations


#endif  // __CSTR_COMPILER__LLVM_HPP