#include <iomanip>
#include <iostream>
#include <fstream>

#include "config.h"

void print_value_summary(const Config::Value v) {
    std::cout << "Summary of value:" << std::endl;
    try {
        std::string v_string = v.as_string();
        std::cout << "    Value as a string = [" << v_string << "]"
            << std::endl;
    } catch(Config::TypeError& te) {
        std::cout << "    Value is not conformable to a string." << std::endl;
    }
    try {
        int64_t v_integer = v.as_integer();
        std::cout << "    Value as an integer = [" << v_integer << "]"
            << std::endl;
    } catch(Config::TypeError& te) {
        std::cout << "    Value is not conformable to an integer."
            << std::endl;
    }
    try {
        double v_float = v.as_float();
        std::cout << std::setprecision(9);
        std::cout << "    Value as a float = [" << v_float << "]" << std::endl;
    } catch(Config::TypeError& te) {
        std::cout << "    Value is not conformable to a float." << std::endl;
    }
    try {
        bool v_boolean = v.as_boolean();
        std::cout << std::boolalpha;
        std::cout << "    Value as a boolean = [" << v_boolean << "]"
            << std::endl;
    } catch(Config::TypeError& te) {
        std::cout << "    Value is not conformable to a boolean." << std::endl;
    }
}

// ============================================================================

void try_to_set(Config::Value& v, const std::string s) {
    try {
        std::cout << "Set value to [" << s << "]." << std::endl;
        v.set_from_string(s);
        print_value_summary(v);
        std::cout << "    serialize as [" << v.serialize() << "]" << std::endl;
    } catch(Config::ParseError& pe) {
        std::cout << "Could not compute valid Value from [" << s << "]: "
            << std::endl;
        std::cout << "    " << pe.what() << std::endl;
    }
}

// ============================================================================

int main(int argc, char *argv[]) {
    Config::Value v;
    print_value_summary(v);

    try_to_set(v, "yes");
    try_to_set(v, "\"yes");
    try_to_set(v, "\"yes\"");
    try_to_set(v, "\"\\\"yes\\\" or \\\"no\\\"\"");
    try_to_set(v, "true");
    try_to_set(v, "false");
    try_to_set(v, "150");
    try_to_set(v, "0");
    try_to_set(v, "-73");
    try_to_set(v, ".01");
    try_to_set(v, "0.01");
    try_to_set(v, "3.141592654");
    try_to_set(v, "-2.718281828");
    try_to_set(v, "123456789.0");
    try_to_set(v, "   3.6e2");
    try_to_set(v, "9.87654321e5");
    try_to_set(v, "1.23456789E5");
    try_to_set(v, "12345678901234567.89    # this is a comment");
    try_to_set(v, "6.022e23");

    std::string file = "yes = \"yes\" # YES\nno=0\nmaybe    =0.5\nwhat=\"\\\"what\\nare\\byou\\ttalking\\nabout?\\\"\"";
    std::cout << std::endl;
    std::cout << "Parse [" << file << "]" << std::endl;
    Config::Group group;
    group.parse_string(file);
    std::vector<std::string> keys = group.keys();
    for (auto it = keys.begin(); it != keys.end(); it++) {
        std::cout << "    " << *it << " = " << group[*it] << std::endl;
    }
    std::cout << "what (as a string) --> " << group["what"].as_string()
        << std::endl;

    file = "parameters.config";
    std::cout << std::endl;
    std::cout << "Parse file \"" << file << "\"" << std::endl;
    group.parse_file(file);
    keys = group.keys();
    std::cout << group;

    return 0;
}
