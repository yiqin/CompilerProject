
#include "symbol_table.hpp"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

#include "location.hh"
#include "symbol.hpp"


namespace parser {


// static

std::vector<Symbol_Table::Ptr> Symbol_Table::all_tables_ = std::vector<Symbol_Table::Ptr>();

void Symbol_Table::print_tables () {
    for (auto& symbol_table : all_tables_) {
        std::cout
            << symbol_table->name() << std::endl
            << "    start: " << symbol_table->loc.begin << std::endl
            << "    end:   " << symbol_table->loc.end   << std::endl
            << std::endl
            << "  name               | type                      | attributes" << std::endl
            //  <-      20        -> | <-         25          -> |
            << "---------------------+---------------------------+-----------------------" << std::endl
            ;
        for (auto& pair : symbol_table->table_) {
            Symbol::Ptr symbol = pair.second;
            std::cout
                << std::left << std::setw(20) << symbol->name() << " | "
                << std::left << std::setw(25) << symbol->type_str() << " | "
                ;

            if (symbol->get(Symbol::Attribute::EXTERN))
                std::cout << "extern";
            if (symbol->get(Symbol::Attribute::FUNCTION_PARAM))
                std::cout << "function parameter";

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}


Symbol_Table::Symbol_Table (std::string&& name, const location& arg_loc)
      : Symbol_Table(std::move(name), arg_loc, nullptr) {}

Symbol_Table::Symbol_Table (
    std::string&& name,
    const location& arg_loc,
    Symbol_Table::Ptr parent
)
      : loc(arg_loc), name_(std::move(name)), parent_(parent) {}

bool Symbol_Table::is_in_this_scope (const std::string& name) {
    return table_.find(name) != std::end(table_);
}

bool Symbol_Table::is_visible (const std::string& name) {
    if (is_in_this_scope(name))
        return true;

    if (parent_ == nullptr)
        return false;

    return parent_->is_visible(name);
}

Symbol::Ptr Symbol_Table::lookup (const std::string& name) {
    auto iter = table_.find(name);
    if (iter != std::end(table_))
        return iter->second;
    return parent_->lookup(name);
}

void Symbol_Table::add (const std::string& name, Symbol::Ptr symbol) {
    table_.emplace(std::make_pair(name, symbol));
}


}  // namespace parser
