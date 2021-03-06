#include <iomanip>
#include <iostream>
#include <fstream>

#include "toml.h"

void print_value_summary(const TOML::Value v) {
    std::cout << "Summary of value:" << std::endl;
    try {
        std::string v_string = v.as_string();
        std::cout << "    Value as a string = [" << v_string << "]"
            << std::endl;
    } catch(TOML::TypeError& te) {
        std::cout << "    Value is not conformable to a string." << std::endl;
    }
    try {
        int64_t v_integer = v.as_integer();
        std::cout << "    Value as an integer = [" << v_integer << "]"
            << std::endl;
    } catch(TOML::TypeError& te) {
        std::cout << "    Value is not conformable to an integer."
            << std::endl;
    }
    try {
        double v_float = v.as_float();
        std::cout << std::setprecision(9);
        std::cout << "    Value as a float = [" << v_float << "]" << std::endl;
    } catch(TOML::TypeError& te) {
        std::cout << "    Value is not conformable to a float." << std::endl;
    }
    try {
        bool v_boolean = v.as_boolean();
        std::cout << std::boolalpha;
        std::cout << "    Value as a boolean = [" << v_boolean << "]"
            << std::endl;
    } catch(TOML::TypeError& te) {
        std::cout << "    Value is not conformable to a boolean." << std::endl;
    }
}

// ============================================================================

void try_to_set(TOML::Value& v, const std::string s) {
    try {
        std::cout << "Set value to [" << s << "]." << std::endl;
        v.set_from_string(s);
        print_value_summary(v);
        std::cout << "    serialize as [" << v.serialize() << "]" << std::endl;
    } catch(TOML::ParseError& pe) {
        std::cout << "Could not compute valid Value from [" << s << "]: "
            << std::endl;
        std::cout << "    " << pe.what() << std::endl;
    }
}

// ============================================================================

int main(int argc, char *argv[]) {
    TOML::Value v;
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
    TOML::Table table;
    table.parse_string(file);
    std::cout << table;
    std::cout << "what (as a string) --> "
        << table.get_scalar("what").as_string() << std::endl;
    try {
        std::cout << "who --> " << table.get_scalar("who") << std::endl;
    } catch (TOML::TableError& err) {
        std::cout << "key \"who\" does not exist" << std::endl;
    }

    file = "parameters.toml";
    std::cout << std::endl;
    std::cout << "Parse file \"" << file << "\"" << std::endl;
    table.parse_file(file);
    std::cout << table.serialize(1);

    std::cout << std::endl;
    std::cout << "Access of specific members:" << std::endl;
    std::cout << "    float2 -- > " << table.get_scalar("float2") << std::endl;
    std::cout << "    array_var --> " << table.get_array("array_var")
        << std::endl;

    std::cout << std::endl;
    std::cout << "Building a table programmatically." << std::endl;
    table.clear();
    v.set("test value");
    table.add("string_var", v);
    try {
        table.add("string_var", v);
        std::cout << " !! Added redundant value." << std::endl;
    } catch (TOML::TableError& te) {
        std::cout << "    Failed to add redundant value." << std::endl;
    }
    try {
        table.add("string var", v);
        std::cout << " !! Added value with invalid key." << std::endl;
    } catch (TOML::TableError& te) {
        std::cout << "    Failed to add value with invalid." << std::endl;
    }
    TOML::Table table2;
    v.set(static_cast<TOML::Integer>(42));
    table2.add("integer", v);
    v.set(3.14);
    table2.add("float", v);
    table.add("\"table with spaces in name\"", table2);
    try {
        table.add("recursion", table);
        std::cout << " !! Added recursive table." << std::endl;
    } catch (TOML::TableError& te) {
        std::cout << "    Failed to add recursive table." << std::endl;
    }
    std::cout << table.serialize(2);

    return 0;
}
