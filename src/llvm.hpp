
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
static std::string alloca_llvm_ir (parser::Symbol::Ptr symbol) {
  return std::string("%") + symbol->name() + " = alloca " + symbol->type_ir() + end_of_line;
}

}

#endif  // __CSTR_COMPILER__LLVM_HPP