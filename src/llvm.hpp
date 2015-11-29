
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "symbol.hpp"


namespace llvm {


extern std::string end_of_line;

template <typename T>
std::string to_string (T t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


class ID_Factory {
  public:
    ID_Factory () : next_id_(0) {}

    std::string get_id () { return to_string(next_id_++); }
    void reset () { next_id_ = 0; }

  private:
    int next_id_;
};


class Label {
  public:
    typedef std::shared_ptr<Label> Ptr;

    Label ()
          : id_("Label_" + id_factory_.get_id()) {}

    // for br
    const std::string name_llvm_ir () const {
      std::string str;
      str = std::string("label %") + id_;
      return str;
    }

    // the destination label for the jump instruction
    const std::string destination_llvm_ir () const {
      std::string str;
      str = id_ + ":" + "\n";
      return str;
    }

  protected:
    static ID_Factory id_factory_;

  private:
    const std::string id_;
};

class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type& type, const std::string& id)
          : type_(type), id_(id) {}

    const parser::Type& type () const { return type_; }

    std::string type_llvm_ir () {
        if (type_ == parser::Type::INT) {
            return "i32";
        }
        return "/undefine type. Please wait./";
    }

    std::string name_llvm_ir () {
        return '%' + id_;
    }

  protected:
    static ID_Factory id_factory_;

  private:
    const parser::Type type_;
    const std::string id_;
};

class Pointer_Register : public Register {
  public:
    typedef std::shared_ptr<Pointer_Register> Ptr;

    Pointer_Register (const parser::Type& type)
          : Register(type, ("P." + id_factory_.get_id())) {}

    Pointer_Register (const parser::Type& type, const std::string& id)
          : Register(type, id) {}

    // <value>
    std::string value_llvm_ir() {
        return type_llvm_ir() + ' ' + name_llvm_ir();
    }

    // <pointer>
    std::string pointer_llvm_ir() {
        return type_llvm_ir() + "* " + name_llvm_ir();
    }
};

class Value_Register : public Register {
  public:
    typedef std::shared_ptr<Value_Register> Ptr;

    Value_Register (const parser::Type& type)
          : Register(type, ("V." + id_factory_.get_id())) {}

    // <value>
    virtual std::string value_llvm_ir() {
      return type_llvm_ir() + " " + name_llvm_ir();
    }
};

class Cond_Register : public Value_Register {
  public:
    std::string value_llvm_ir() {
      return std::string("i1 ") + name_llvm_ir();
    }
};


// Memory Access and Addressing Operations
// alloca <pointer>
std::string alloca_instruction (parser::Symbol::Ptr symbol);

// alloca <pointer>
std::string alloca_instruction (Pointer_Register::Ptr op);

// Return value
// <value> = load <pointer>
std::string load_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2);

// store <value>, int
std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1);

// store <value>, <pointer>
std::string store_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2);


// Binary Operations


// Other Operations


// br instruction
// br label <dest>
std::string br_instruction (Label::Ptr label_1);

// br i1 <cond>, label <iftrue>, label<iffailure>
std::string br_instruction (llvm::Value_Register::Ptr cond, Label::Ptr label_1, Label::Ptr label_2);


}


#endif  // __CSTR_COMPILER__LLVM_HPP
