
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <string>
#include "symbol.hpp"
#include <iostream>

namespace llvm {

// We want the register number is only used by one time.
// They are never the same.
static int value_register_number = 0;
static int get_value_register_number() {
  return value_register_number++;
};

static int pointer_register_number = 0;
static int get_pointer_register_number() {
  return pointer_register_number++;
}

static int label_number = 0;
static int get_label_number() {
  return label_number++;
};

// Something wrong with this case. 
// Fails to reset the code. 
// Create a headache to the unit test.
static void reset() {
  llvm::value_register_number = 0;
  llvm::pointer_register_number = 0;
  llvm::label_number = 0;
};

static std::string end_of_line = ", align 4\n";



class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type type) 
           : type_(type), id_(std::string("R.")+std::to_string(get_pointer_register_number())) {}
           
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
       
  private:
    const parser::Type type_;
    const std::string id_;
};

class Pointer_Register : public Register {
  public:
    typedef std::shared_ptr<Pointer_Register> Ptr;  
    
    Pointer_Register (const parser::Type type) 
          : Register(type, (std::string("R.")+std::to_string(get_pointer_register_number()))) {}

    Pointer_Register (const parser::Type type, const std::string id)
          : Register(type, id) {}

    // <value>
    std::string value_llvm_ir() {
      return type_llvm_ir() + " " + name_llvm_ir();
    }
    
    // <pointer>
    std::string pointer_llvm_ir() {
      return type_llvm_ir() + "* " + name_llvm_ir();
    }
    
};

class Value_Register : public Register {
  public:
    typedef std::shared_ptr<Value_Register> Ptr; 
     
    Value_Register (const parser::Type type) 
          : Register(type, (std::string("V.")+std::to_string(get_value_register_number()))) {}

    // <value>
    std::string value_llvm_ir() {
      return type_llvm_ir() + " " + name_llvm_ir();
    }  
    
};

// Create pointer registers and value registers
static Pointer_Register::Ptr new_pointer_register(const parser::Type type) {
  Pointer_Register::Ptr tmp = std::make_shared<Pointer_Register>(type);
  return tmp;
};

static Pointer_Register::Ptr new_pointer_register(const parser::Type type, const std::string id) {
  Pointer_Register::Ptr tmp = std::make_shared<Pointer_Register>(type, id);
  return tmp;
};

static Value_Register::Ptr new_value_register(const parser::Type type) {
  Value_Register::Ptr tmp = std::make_shared<Value_Register>(type);
  return tmp;
};


// Memory Access and Addressing Operations
// alloca <pointer>
static std::string alloca_instruction (parser::Symbol::Ptr symbol) {
  return std::string("%") + symbol->name() + " = alloca " + symbol->type_llvm_ir() + end_of_line;
}

// alloca <pointer>
static std::string alloca_instruction (Pointer_Register::Ptr op) {
  return op->name_llvm_ir() + " = alloca " + op->type_llvm_ir() + end_of_line;
}

// Return value
// <value> = load <pointer>
static std::string load_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {  
  std::string ir;
  ir += op_1->name_llvm_ir();
  ir += " = load ";
  ir += op_2->pointer_llvm_ir();
  ir += end_of_line;
  return ir;
}

// store <value>, int
static std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1) {
  std::string ir;
  ir += std::string("store i32 ") + std::to_string(integer_value);
  ir += ", ";
  ir += op_1->pointer_llvm_ir();
  ir += "\n";
  return ir;
}

// store <value>, <pointer>
static std::string store_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {
  std::string ir;
  ir += std::string("store ") + op_1->value_llvm_ir();
  ir += ", ";
  ir += op_2->pointer_llvm_ir();
  ir += "\n";
  return ir;
}


// Binary Operations


// Other Operations
  

}


#endif  // __CSTR_COMPILER__LLVM_HPP