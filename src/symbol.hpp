
#ifndef __CSTR_COMPILER__SYMBOL_HPP
#define __CSTR_COMPILER__SYMBOL_HPP


#include <cstddef>

#include <bitset>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


namespace parser {


enum class Type {
    INT,
    STRING,
};


class Symbol {
  public:
    typedef std::shared_ptr<Symbol> Ptr;

    enum class Attribute : std::size_t {
        EXTERN,
        GLOBAL,
        FUNCTION_PARAM,
        kSize
    };

    Symbol (std::string&& name) : name_(std::move(name)), access_count_(0) {}
    virtual ~Symbol() {}

    const std::string& name             () const { return name_; }
    const Type         type             () const { return type_; }
    const int          access_count () const { return access_count_; }

    void name (std::string&& value) { name_ = std::move(value); }
    void type (Type          value) { type_ = value; }
    void increment_access_count () { ++access_count_; }

    virtual std::string type_str () const { return type_ == Type::INT ? "int" : "string"; }
    virtual std::string type_llvm_ir () const { return type_ == Type::INT ? "i32" : "i8*"; }

    const bool get (Attribute attribute) const {
        return attributes_[static_cast<std::size_t>(attribute)];
    }

    void set (Attribute attribute) {
        attributes_.set(static_cast<std::size_t>(attribute), true);
    }

    void unset (Attribute attribute) {
        attributes_.set(static_cast<std::size_t>(attribute), false);
    }

    // // TODO (Emery): Remove for production.
    // virtual void print_semantic_action () {
    //     std::cout << "- declare variable '" << name_ << "' of type "
    //         << (type_ == Type::INT ? "integer" : "string") << std::endl;
    // }

  protected:
    std::string name_;
    Type type_;
    int access_count_;

    std::bitset<static_cast<std::size_t>(Attribute::kSize)> attributes_;
};


class Function : public Symbol {
  public:
    typedef std::shared_ptr<Function> Ptr;
    typedef std::vector<Symbol::Ptr> Argument_List;

    Function (std::string&& name) : Symbol(std::move(name)) {}

    virtual std::string type_str () const {
        std::ostringstream oss;
        oss << "function";
        for (auto& symbol : argument_list_) {
            oss << ", " << (symbol->type() == Type::INT ? "int" : "string");
        }
        return oss.str();
    }

    Argument_List& argument_list () { return argument_list_; }

    // // TODO (Emery): Remove for production.
    // virtual void print_semantic_action () {
    //     std::cout << "- declare function '" << name_ << "'" << std::endl;
    // }

  private:
    Argument_List argument_list_;
};


typedef std::vector<Symbol::Ptr> Symbol_List;


}  // namespace parser


#endif  // __CSTR_COMPILER__SYMBOL_HPP
