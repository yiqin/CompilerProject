
#ifndef _CSTR_COMPILER__SYMBOL_TABLE_HPP
#define _CSTR_COMPILER__SYMBOL_TABLE_HPP


#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "location.hh"
#include "symbol.hpp"

namespace parser {


class Symbol_Table {
  public:
    typedef std::shared_ptr<Symbol_Table> Ptr;

    static void print_tables ();

    // Delete all default constructors/assignment operators.
    Symbol_Table ()                                = delete;
    Symbol_Table (const Symbol_Table&)             = delete;
    Symbol_Table (Symbol_Table&&)                  = delete;
    Symbol_Table& operator = (const Symbol_Table&) = delete;
    Symbol_Table& operator = (Symbol_Table&&)      = delete;

    // Factory constructor.
    template <typename ...T>
    static Ptr construct (T&&... t) {
        Ptr p (new Symbol_Table(std::forward<T>(t)...));
        all_tables_.push_back(p);
        return p;
    }

    const std::string& name () const { return name_; }
    Ptr parent () { return parent_; }

    bool is_in_this_scope (const std::string& name);
    bool is_visible       (const std::string& name);
    Symbol::Ptr lookup    (const std::string& name);

    void add (const std::string& name, Symbol::Ptr symbol);

    location loc;

  private:
    // Constructors private to control new object creation.
    Symbol_Table (std::string&& name, const location& arg_loc);
    Symbol_Table (std::string&& name, const location& arg_loc, Ptr parent);

    std::string name_;
    Ptr parent_;

    std::unordered_map<std::string, Symbol::Ptr> table_;

    static std::vector<Ptr> all_tables_;
};


}  // namespace parser


#endif  // _CSTR_COMPILER__SYMBOL_TABLE_HPP
