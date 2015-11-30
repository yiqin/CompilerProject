
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "symbol.hpp"


template <typename T>
std::string to_string (T t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


namespace llvm {


extern std::string end_of_line;


class ID_Factory {
  public:
    ID_Factory () : next_id_(0) {}

    std::string get_id () { return to_string(next_id_++); }
    void reset () { next_id_ = 0; }

  private:
    int next_id_;
};

class String {
  public:
    typedef std::shared_ptr<String> Ptr;

    // Delete all default constructors/assignment operators.
    String             ()              = delete;
    String             (const String&) = delete;
    String             (String&&)      = delete;
    String& operator = (const String&) = delete;
    String& operator = (String&&)      = delete;

    // Factory constructor.
    template <typename... T>
    static Ptr construct (T&&... t) {
        Ptr p (new String(std::forward<T>(t)...));
        all_strings_.push_back(p);
        return p;
    }

    static const std::vector<Ptr> all_strings () { return all_strings_; }
    static void clear_store () { all_strings_.clear(); }

    const std::string& id () const {
        return id_;
    }

    const std::string& value () const {
        return value_;
    }

  protected:
    static ID_Factory id_factory_;

  private:
    // Constructors private to control new object creation.
    String (const std::string& value)
          : value_(value),
            id_("@.str_" + id_factory_.get_id()) {}

    const std::string id_;
    const std::string value_;

    static std::vector<Ptr> all_strings_;
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
      switch (type_) {
        case parser::Type::INT:
          return "i32";
        case parser::Type::STRING:
          return "i8*";
      }
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

    Value_Register (const parser::Type& type, const std::string& id)
          : Register(type, id) {}
          
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

// store int, <pointer>
std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1);

// store string, <pointer>
std::string store_instruction (llvm::String::Ptr string_1, llvm::Pointer_Register::Ptr op_1);

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
