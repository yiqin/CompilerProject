
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <string>
#include "symbol.hpp"

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
}

static void reset() {
  register_number = 0;
  label_number = 0;
}



class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type type) 
          : type_(type), id_(get_register_number()) {}
    
    std::string type_ir () {
      if (type_ == parser::Type::INT) {
        return "i32";
      }
      return "/undefine type. Please wait./";
    }
    
    std::string name_llvm_ir () {
      return "%"+std::to_string(id_);
    }
    
    std::string value_llvm_ir() {
      return type_ir() + " " + name_llvm_ir();
    }
    
    std::string pointer_llvm_ir() {
      return type_ir() + "* " + name_llvm_ir();
    }
    
  private:
    const parser::Type type_;
    const int id_;
};


}

#endif  // __CSTR_COMPILER__LLVM_HPP