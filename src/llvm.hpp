
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <iostream>
#include <memory>
#include <string>

#include "symbol.hpp"


namespace llvm {


std::string end_of_line = ", align 4\n";


class ID_Factory {
  public:
    ID_Factory () : next_id_(0) {}

    std::string get_id () { return std::to_string(next_id_++); }
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

// Create label
static Label::Ptr new_label() {
  Label::Ptr tmp = std::make_shared<Label>();
  return tmp;
};

class Register {
  public:
    typedef std::shared_ptr<Register> Ptr;

    Register (const parser::Type& type, const std::string& id)
          : type_(type), id_(id) {}

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
          : Register(type, ("R." + id_factory_.get_id())) {}

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
};

// alloca <pointer>
static std::string alloca_instruction (Pointer_Register::Ptr op) {
  return op->name_llvm_ir() + " = alloca " + op->type_llvm_ir() + end_of_line;
};

// Return value
// <value> = load <pointer>
static std::string load_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {
  std::string ir;
  ir += op_1->name_llvm_ir();
  ir += " = load ";
  ir += op_2->pointer_llvm_ir();
  ir += end_of_line;
  return ir;
};

// store <value>, int
static std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1) {
  std::string ir;
  ir += std::string("store i32 ") + std::to_string(integer_value);
  ir += ", ";
  ir += op_1->pointer_llvm_ir();
  ir += "\n";
  return ir;
};

// store <value>, <pointer>
static std::string store_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2) {
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
static std::string br_instruction (Label::Ptr label_1) {
  return std::string("br ") + label_1->name_llvm_ir() + "\n";
};

// br i1 <cond>, label <iftrue>, label<iffailure>
static std::string br_instruction (llvm::Value_Register::Ptr cond, Label::Ptr label_1, Label::Ptr label_2) {
   return std::string("br i1 ") + cond->name_llvm_ir() + ", " + label_1->name_llvm_ir() + ", " + label_2->name_llvm_ir() + "\n";
};


}


#endif  // __CSTR_COMPILER__LLVM_HPP
