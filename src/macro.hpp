
#ifndef __CSTR_COMPILER__MACRO_HPP
#define __CSTR_COMPILER__MACRO_HPP


#include <memory>
#include <string>
#include <vector>


namespace preprocessor {


class Macro {
  public:
    Macro (std::string&& name);
    virtual ~Macro () {}

    typedef std::shared_ptr<Macro> Ptr;

    const std::string& name () const { return name_; }
    const std::string& body () const { return body_; }

    void body (std::string&& val);

  protected:
    std::string name_;
    std::string body_;
};


class Macro_Function : public Macro {
  public:
    typedef std::vector<std::string> Argument_List;

    Macro_Function (std::string&& name);

    Argument_List& argument_names () { return argument_names_; }

    std::string resolve (const Argument_List& args) const;

  protected:
    Argument_List argument_names_;
};


}  // namespace preprocessor


#endif  // __CSTR_COMPILER__MACRO_HPP
