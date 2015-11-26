
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
          : type_(type), register_number_of_result_(get_register_number()) {}
    
  private:
    const parser::Type type_;
    const int register_number_of_result_;
};


}

#endif  // __CSTR_COMPILER__LLVM_HPP