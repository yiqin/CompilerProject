
#ifndef __CSTR_COMPILER__LLVM_HPP
#define __CSTR_COMPILER__LLVM_HPP

#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast.hpp"
#include "symbol.hpp"


template <typename T>
std::string to_string (T t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


namespace llvm {


class ID_Factory {
  public:
    ID_Factory () : next_id_(0) {}

    std::string get_id () { return to_string(next_id_++); }
    void reset () { next_id_ = 0; }

  private:
    int next_id_;
};

// class String {
//   public:
//     typedef std::shared_ptr<String> Ptr;

//     // Delete all default constructors/assignment operators.
//     String             ()              = delete;
//     String             (const String&) = delete;
//     String             (String&&)      = delete;
//     String& operator = (const String&) = delete;
//     String& operator = (String&&)      = delete;

//     // Factory constructor.
//     template <typename... T>
//     static Ptr construct (T&&... t) {
//         Ptr p (new String(std::forward<T>(t)...));
//         all_strings_.push_back(p);
//         return p;
//     }

//     static const std::vector<Ptr> all_strings () { return all_strings_; }
//     static void clear_store () { all_strings_.clear(); }

//     const std::string& id () const {
//         return id_;
//     }

//     const std::string& value () const {
//         return value_;
//     }

//   protected:
//     static ID_Factory id_factory_;

//   private:
//     // Constructors private to control new object creation.
//     String (const std::string& value)
//           : value_(value),
//             id_("@.str." + id_factory_.get_id()) {}

//     const std::string id_;
//     const std::string value_;

//     static std::vector<Ptr> all_strings_;
// };

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


class LLVM_Generator : public ast::Code_Generator {
  public:
    LLVM_Generator (std::ostream& out) : out_(out), const_string_next_id_(0) {}

    void indentation (std::string&& value) { indentation_ = std::move(value); }

    // void visit (ast::Node::Ptr                   node) override;
    // void visit (ast::Expression::Ptr             node) override;
    // void visit (ast::Terminal::Ptr               node) override;
    void visit (ast::Declaration_List::Ptr       node) override;
    void visit (ast::Variable::Ptr               node) override;
    void visit (ast::Const_Integer::Ptr          node) override;
    void visit (ast::Const_String::Ptr           node) override;
    void visit (ast::Unary_Expression::Ptr       node) override;
    void visit (ast::Binary_Expression::Ptr      node) override;
    void visit (ast::Condition::Ptr              node) override;
    void visit (ast::Assignment::Ptr             node) override;
    void visit (ast::Function_Call::Ptr          node) override;
    void visit (ast::Instruction::Ptr            node) override;
    void visit (ast::Expression_Instruction::Ptr node) override;
    void visit (ast::Cond_Instruction::Ptr       node) override;
    void visit (ast::While_Instruction::Ptr      node) override;
    void visit (ast::Do_Instruction::Ptr         node) override;
    void visit (ast::For_Instruction::Ptr        node) override;
    void visit (ast::Return_Instruction::Ptr     node) override;
    void visit (ast::Compound_Instruction::Ptr   node) override;
    void visit (ast::Function_Declaration::Ptr   node) override;
    void visit (ast::Function_Definition::Ptr    node) override;

  private:
    std::ostream& out_;

    std::unordered_map<ast::Expression::Ptr, std::string> register_reference_;
    std::unordered_map<parser::Symbol::Ptr, std::size_t>  variable_counts_;

    std::vector<std::pair<std::string, std::string>> const_strings_;
    std::size_t const_string_next_id_;
    bool need_string_functions_ = false;
    bool string_functions_declared_ = false;
    std::unordered_set<std::string> strings_to_free_;

    std::size_t indent_level_ = 0;
    std::string indentation_;

    std::size_t increment_var_count_ (parser::Symbol::Ptr symbol) {
        auto iter = variable_counts_.find(symbol);
        if (iter == std::end(variable_counts_)) {
            return variable_counts_[symbol] = 1;
        } else {
            return ++variable_counts_[symbol];
        }
    }

    void apply_indent_ () {
        for (std::size_t i = 0; i < indent_level_; ++i) {
            out_ << indentation_;
        }
    }

    void emit_label_ (const Label& label) {
        if (indent_level_ > 0) {
            --indent_level_;
            apply_indent_();
            out_ << label.destination_llvm_ir();
            ++indent_level_;
        } else {
            out_ << label.destination_llvm_ir();
        }
    }
};


std::string type (parser::Type t);

// class Register {
//   public:
//     typedef std::shared_ptr<Register> Ptr;

//     Register (const parser::Type& type, const std::string& id)
//           : type_(type), id_(id) {}

//     const parser::Type& type () const { return type_; }

//     std::string type_llvm_ir () {
//       switch (type_) {
//         case parser::Type::INT:
//           return "i32";
//         case parser::Type::STRING:
//           return "i8*";
//       }
//     }

//     std::string name_llvm_ir () {
//         return '%' + id_;
//     }

//   protected:
//     static ID_Factory id_factory_;

//   private:
//     const parser::Type type_;
//     const std::string id_;
// };

// class Pointer_Register : public Register {
//   public:
//     typedef std::shared_ptr<Pointer_Register> Ptr;

//     Pointer_Register (const parser::Type& type)
//           : Register(type, ("P." + id_factory_.get_id())) {}

//     // <pointer>
//     std::string pointer_llvm_ir() {
//         return type_llvm_ir() + "* " + name_llvm_ir();
//     }
// };

// class Value_Register : public Register {
//   public:
//     typedef std::shared_ptr<Value_Register> Ptr;

//     Value_Register (const parser::Type& type)
//           : Register(type, ("V." + id_factory_.get_id())) {}

//     Value_Register (const parser::Type& type, const std::string& id)
//           : Register(type, id) {}

//     // <value>
//     virtual std::string value_llvm_ir() {
//       return type_llvm_ir() + " " + name_llvm_ir();
//     }
// };

// class Cond_Register : public Value_Register {
//   public:
//     std::string value_llvm_ir() {
//       return std::string("i1 ") + name_llvm_ir();
//     }
// };


// Memory Access and Addressing Operations
// alloca <pointer>
// std::string alloca_instruction (parser::Symbol::Ptr symbol);

// // alloca <pointer>
// std::string alloca_instruction (Pointer_Register::Ptr op);

// Return value
// <value> = load <pointer>
// std::string load_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2);

// store int, <pointer>
// std::string store_instruction (int integer_value, llvm::Pointer_Register::Ptr op_1);

// store string, <pointer>
// std::string store_instruction (llvm::String::Ptr string_1, llvm::Pointer_Register::Ptr op_1);

// store <value>, <pointer>
// std::string store_instruction (llvm::Value_Register::Ptr op_1, llvm::Pointer_Register::Ptr op_2);


// Binary Operations


// Other Operations


// br instruction
// br label <dest>
std::string br_instruction (const Label& label_1);

// br i1 <cond>, label <iftrue>, label<iffailure>
std::string br_instruction (const std::string& cond, const Label& label_1, const Label& label_2);

}


#endif  // __CSTR_COMPILER__LLVM_HPP
