
#include <cassert>

#include <string>
#include <utility>

#include "macro.hpp"


namespace preprocessor {


// Macro - member function definitions


Macro::Macro (std::string&& name) : name_(std::move(name)) {}

void Macro::body (std::string&& val) {
    body_ = std::move(val);
}


// Macro_Function - member function definitions

Macro_Function::Macro_Function (std::string&& name) : Macro(std::move(name)) {}

std::string Macro_Function::resolve (const Argument_List& args) const {
    assert(args.size() == argument_names_.size());
    std::string result = body_;

    for (Argument_List::size_type i = 0; i < argument_names_.size(); ++i) {
        const std::string& arg_name  = argument_names_[i];
        const std::string& arg_value = args[i];
        for (
            std::string::size_type position = result.find(arg_name);
            position != std::string::npos;
            position = result.find(arg_name)
        ) {
            result.replace(position, arg_name.size(), arg_value);
        }
    }

    return result;
}


}  // namespace preprocessor
