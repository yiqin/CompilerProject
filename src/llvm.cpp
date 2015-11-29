
#include "llvm.hpp"

#include <string>


namespace llvm {

// The align 4 ensures that the address will be a multiple of 4
std::string alignment_llvm_ir (parser::Type type) {
  switch (type) {
    case parser::Type::INT:
      return "align 4";
    case parser::Type::STRING:
      return "align 8";
  }
}

ID_Factory Label::id_factory_;
ID_Factory Register::id_factory_;


// Memory Access and Addressing Operations
// alloca <pointer>
std::string alloca_instruction (parser::Symbol::Ptr symbol) {
  return std::string("%") + symbol->name() + " = alloca " + symbol->type_llvm_ir() + ", " + alignment_llvm_ir(symbol->type()) + "\n";
};

// alloca <pointer>
std::string alloca_instruction (Pointer_Register::Ptr op) {
  return op->name_llvm_ir() + " = alloca " + op->type_llvm_ir() + ", " + alignment_llvm_ir(op->type()) + "\n";
};

// Return value
// <value> = load <pointer>
std::string load_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {
  std::string ir;
  ir += op_1->name_llvm_ir();
  ir += " = load ";
  ir += op_2->pointer_llvm_ir();
  ir += ", " + alignment_llvm_ir(op_1->type()) + "\n";
  return ir;
};

// store <value>, int
std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1) {
  std::string ir;
  ir += std::string("store i32 ") + to_string(integer_value);
  ir += ", ";
  ir += op_1->pointer_llvm_ir();
  ir += "\n";
  return ir;
};

// store <value>, <pointer>
std::string store_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {
  std::string ir;
  ir += std::string("store ") + op_1->value_llvm_ir();
  ir += ", ";
  ir += op_2->pointer_llvm_ir();
  ir += "\n";
  return ir;
};


// Binary Operations


// Other Operations


// br instruction
// br label <dest>
std::string br_instruction (Label::Ptr label_1) {
  return std::string("br ") + label_1->name_llvm_ir() + "\n";
};

// br i1 <cond>, label <iftrue>, label<iffailure>
std::string br_instruction (llvm::Value_Register::Ptr cond, Label::Ptr label_1, Label::Ptr label_2) {
   return std::string("br i1 ") + cond->name_llvm_ir() + ", " + label_1->name_llvm_ir() + ", " + label_2->name_llvm_ir() + "\n";
};

}  // namespace llvm
