/* A simple configuration file parser. */

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "config.h"

// ============================================================================
// General parsing functions that will be used in both Values and Tables

// Advance the iterator while there is white space.
void consume_whitespace(Config::string_it& it, const Config::string_it& end) {
    while (it != end && (*it == ' ' || *it == '\t')) {
        it++;
    }
}

// ----------------------------------------------------------------------------

// Advance the iterator to the end of line (allowing for comments).  If
// anything other than white space or comments is found, this is a ParseError.
void consume_end_of_line(Config::string_it& it, const Config::string_it& end) {
    consume_whitespace(it, end);
    if (it != end && *it != '#') {
        throw Config::ParseError("Trailing characters.");
    }
}

// ----------------------------------------------------------------------------

// Check if the character is a valid digit (0-9).  While functions for these
// things exist, I don't want to tangle with issues of locale, so I hardcoded
// my own.
static bool is_digit(const char c) {
    switch(c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

// ----------------------------------------------------------------------------

// Convert a character to a digit, or raise a ParseError if the character is
// not equivalent to a digit.  While functions for these things exist, I don't
// want to tangle with issues of locale, so I hardcoded my own.
static unsigned to_digit(const char c) {
    switch(c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default:
              std::string message = "Character \"";
              message.append(1, c);
              message.append("\" is not a digit.");
              throw Config::ParseError(message);
    }
}

// ============================================================================
// Value ______________________________________________________________________

// Clear the Value -- erase the internal values and set it to be nonconformable
void Config::Value::clear() {
    // Not conformable to anything
    is_conformable_to_string = false;
    is_conformable_to_integer = false;
    is_conformable_to_float = false;
    is_conformable_to_boolean = false;
    // Wipe the value(s)
    value_as_string = "";
    value_as_integer = 0;
    value_as_float = 0.0;
    value_as_boolean = false;
}

// ----------------------------------------------------------------------------

// Attempt to parse the value as a String.  Return the String or raise a
// ParseError if parsing fails.
Config::String Config::Value::parse_string(
        Config::string_it& it, const Config::string_it& end) {
    if (*it != '"') {
        throw Config::ParseError("Unable to parse as a string.");
    }
    it++;
    Config::String temp_string = "";
    while (it != end) {
        if (*it == '\\') {
            it++;
            if (*it == '"') {
                temp_string += "\"";
                it++;
            } else if (*it == '\\') {
                temp_string += "\\";
                it++;
            } else {
                // TODO -- Check the escape list for TOML
                std::string message = "Unknown escape character \"\\";
                message.append(1, *it);
                message.append("\".");
                throw Config::ParseError(message);
            }
        } else if (*it == '"') {
            break;
        } else {
            temp_string += *it;
            it++;
        }
    }
    if (*it != '"') {
        throw Config::ParseError("Unable to parse as a string.");
    }
    it++;
    return temp_string;
}

// ----------------------------------------------------------------------------

// Attempt to parse the value as a Boolean.  Return the Boolean or raise a
// ParseError if parsing fails.
Config::Boolean Config::Value::parse_boolean(
        Config::string_it& it, const Config::string_it& end) {
    Config::Boolean temp_bool;
    if (std::string(it, it+4) == "true") {
        it = it + 4;
        temp_bool = true;
    } else if (std::string(it, it+5) == "false") {
        it = it + 5;
        temp_bool = false;
    } else {
        throw Config::ParseError("Unable to parse as a boolean.");
    }
    return temp_bool;
}

// ----------------------------------------------------------------------------

// Attempt to parse the value as a number.  Return the Number or raise a
// ParseError if parsing fails.
// TODO -- handle underscore separators
Config::Number Config::Value::parse_number(
        Config::string_it& it, const Config::string_it& end) {
    Config::Number temp_number;
    temp_number.valid_integer = false;
    temp_number.integer_value = 0;
    temp_number.valid_float = false;
    temp_number.float_value = 0.0;
    // sign
    Config::Integer sign;
    if (*it == '-') {
        sign = -1;
        it++;
    } else if (*it == '+') {
        sign = 1;
        it++;
    } else if (*it == '.' || is_digit(*it)) {
        sign = 1;
    } else {
        throw Config::ParseError("Unable to parse as a number.");
    }
    // integer part
    Config::Integer ipart = 0;
    while (it != end && is_digit(*it)) {
        ipart = 10 * ipart + to_digit(*it);
        it++;
    }
    // decimal
    Config::Float dpart = 0;
    Config::Float shift = 0.1;
    if (it != end && *it == '.') {
        it++;
        while (it != end && is_digit(*it)) {
            dpart += shift * to_digit(*it);
            shift *= 0.1;
            it++;
        }
    }
    // exponent (scientific notation)
    Config::Integer e_sign;
    Config::Integer exponent = 0;
    if (it != end && (*it == 'e' || *it == 'E')) {
        it++;
        if (*it == '-') {
            e_sign = -1;
            it++;
        } else if (*it == '+') {
            e_sign = 1;
            it++;
        } else if (is_digit(*it)) {
            e_sign = 1;
        } else {
            throw Config::ParseError("Invalid exponent in number.");
        }
        while (it != end && is_digit(*it)) {
            exponent = 10 * exponent + to_digit(*it);
            it++;
        }
        exponent *= e_sign;
    }
    // Construct the number
    if (dpart == 0 && exponent == 0) {
        // This is really an integer, and may also be a float
        Config::Integer as_integer = sign * ipart;
        Config::Float as_float = static_cast<Config::Float>(as_integer);
        if (as_integer == as_float) {
            temp_number.float_value = as_float;
            temp_number.valid_float = true;
        }
        temp_number.integer_value = as_integer;
        temp_number.valid_integer = true;
    } else {
        // This is really a float, and may also be an integer
        Config::Float as_float = sign *
            (static_cast<Config::Float>(ipart) + dpart) *
            std::pow(10.0, exponent);
        Config::Integer as_integer = static_cast<Config::Integer>(as_float);
        if (as_float == as_integer) {
            temp_number.integer_value = as_integer;
            temp_number.valid_integer = true;
        }
        temp_number.float_value = as_float;
        temp_number.valid_float = true;
    }
    return temp_number;
}

// ----------------------------------------------------------------------------

// Analyze the given input string.  If it is a valid value, set the Value to
// have the appropriate internal values and flags.  Otherwise, raise a
// ParseError.
void Config::Value::analyze(const std::string input_string) {
    // Clear the current internal values and flags
    clear();

    // Get the iterators for the input string
    Config::string_it it = input_string.begin();
    const Config::string_it end = input_string.end();

    // Trim leading whitespace
    consume_whitespace(it, end);

    // Ensure there is something (non-comment) left in the string
    if (it == end || *it == '#') {
        throw Config::ParseError("Empty value.");
    }

    // Choose which type to parse
    if (*it == '"') {
        // This is either a String or nothing
        Config::String temp_string = parse_string(it, end);
        consume_end_of_line(it, end);
        // Save it
        value_as_string = temp_string;
        is_conformable_to_string = true;
    } else if (*it == 't' || *it == 'f') {
        // This is either a Boolean or nothing
        Config::Boolean temp_boolean = parse_boolean(it, end);
        consume_end_of_line(it, end);
        // Save it
        value_as_boolean = temp_boolean;
        is_conformable_to_boolean = true;
    } else if (*it == '-' || *it == '+' || *it == '.' ||
            is_digit(*it)) {
        // This is either an Integer, a Float, both, or nothing
        Config::Number temp_number = parse_number(it, end);
        consume_end_of_line(it, end);
        // Save it
        value_as_integer = temp_number.integer_value;
        value_as_float = temp_number.float_value;
        is_conformable_to_integer = temp_number.valid_integer;
        is_conformable_to_float = temp_number.valid_float;
    } else {
        // This is nothing
        throw Config::ParseError("Unable to parse \"" + input_string +
                "\" to a value.");
    }
}

// ----------------------------------------------------------------------------

// Construct an empty Value
// -- note: yes, an initialization list would be slightly more efficient.  But
//    considering all my internal data is basic types, the performance
//    difference is, for all intents and purposes, zero.  This is easier to
//    write and to maintain.
Config::Value::Value() {
    clear();
}

// ----------------------------------------------------------------------------

// Construct a Value by analyzing an input string
Config::Value::Value(const std::string input_string) {
    analyze(input_string);
}

// ----------------------------------------------------------------------------

// Set the Value by analyzing an input string
void Config::Value::set_from_string(const std::string input_string) {
    analyze(input_string);
}

// ----------------------------------------------------------------------------

// Set the Value from a String
// TODO -- Verify that this simple method works; I may have to surround the
//         String with double quotes and run it through analyze().
void Config::Value::set(const Config::String s) {
    clear();
    value_as_string = s;
    is_conformable_to_string = true;
}

// ----------------------------------------------------------------------------

// Set the Value from an Integer (may also be conformable to a Float)
void Config::Value::set(const Config::Integer i) {
    clear();
    value_as_integer = i;
    is_conformable_to_integer = true;
    Config::Float temp_float = static_cast<Config::Float>(i);
    if (temp_float == i) {
        value_as_float = temp_float;
        is_conformable_to_float = true;
    }
}

// ----------------------------------------------------------------------------

// Set the Value from a Float (may also be conformable to an Integer)
void Config::Value::set(const Config::Float f) {
    clear();
    value_as_float = f;
    is_conformable_to_float = true;
    Config::Integer temp_integer = static_cast<Config::Integer>(f);
    if (temp_integer == f) {
        value_as_integer = temp_integer;
        is_conformable_to_integer = true;
    }
}

// ----------------------------------------------------------------------------

// Set the Value from a Boolean
void Config::Value::set(const Config::Boolean b) {
    clear();
    value_as_boolean = b;
    is_conformable_to_boolean = true;
}

// ----------------------------------------------------------------------------

// Return the Value as a String
Config::String Config::Value::as_string() const {
    if (is_conformable_to_string) {
        return value_as_string;
    } else {
        throw Config::TypeError("Value cannot be converted to a string.");
    }
}

// ----------------------------------------------------------------------------

// Return the Value as an Integer
Config::Integer Config::Value::as_integer() const {
    if (is_conformable_to_integer) {
        return value_as_integer;
    } else {
        throw Config::TypeError("Value cannot be converted to an integer.");
    }
}

// ----------------------------------------------------------------------------

// Return the Value as a Float
Config::Float Config::Value::as_float() const {
    if (is_conformable_to_float) {
        return value_as_float;
    } else {
        throw Config::TypeError("Value cannot be converted to an integer.");
    }
}

// ----------------------------------------------------------------------------

// Return the Value as a Boolean
Config::Boolean Config::Value::as_boolean() const {
    if (is_conformable_to_boolean) {
        return value_as_boolean;
    } else {
        throw Config::TypeError("Value cannot be converted to an boolean.");
    }
}

// ----------------------------------------------------------------------------

// Convert the Value to a std::string as if writing a new config file
std::string Config::Value::serialize() const {
    if (is_conformable_to_boolean) {
        // Write as a Boolean
        if (value_as_boolean) {
            return "true";
        } else {
            return "false";
        }
    } else if (is_conformable_to_integer) {
        // Write as an Integer (anything conformable to both Integer and Float
        // will appear as an Integer because Integers go before Floats)
        std::stringstream ss;
        ss << value_as_integer;
        return ss.str();
    } else if (is_conformable_to_float) {
        // Write as a Float
        std::stringstream ss;
        ss << std::setprecision(15) << value_as_float;
        return ss.str();
    } else if (is_conformable_to_string) {
        // Write as a String
        Config::string_it it = value_as_string.begin();
        Config::string_it end = value_as_string.end();
        std::string output = "\""; // Surround with double-quotes
        while (it != end) { // Fix escape sequences
            // TODO -- Check the escape list for TOML
            if (*it == '"' || *it == '\\') {
                output.append(1, '\\');
            }
            output.append(1, *it);
            it++;
        }
        output += "\""; // Surround with double-quotes
        return output;
    } else {
        // Not actually a valid Value
        throw Config::ValueError("Value cannot be serialized.");
    }
}

// ----------------------------------------------------------------------------

// Write a Value to a stream
std::ostream& Config::operator<< (std::ostream& sout, const Config::Value& v) {
    sout << v.serialize();
    return sout;
}

// ============================================================================
// Group ______________________________________________________________________

// Advance the iterator across a valid key.  Raise a ParseError if there is no
// valid key present at the iterator.
// TODO -- better method of getting valid keys
void Config::Group::consume_key(
        Config::string_it& it, const Config::string_it& end) {
    while (it != end && (*it != ' ' && *it != '\t' && *it != '=')) {
        it++;
    }
}

// ----------------------------------------------------------------------------

// Parse a Group from an input string
void Config::Group::parse_string(const std::string s) {
    // Load into a stringstream and parse that stream
    std::istringstream iss(s);
    parse_stream(iss);
}

// ----------------------------------------------------------------------------

// Parse a Group from a file (specified by the file name)
void Config::Group::parse_file(const std::string filename) {
    // Open the file as a filestream and parse that stream
    std::ifstream fin;
    fin.open(filename);
    parse_stream(fin);
    fin.close();    // Don't forget to close the file!
}

// ----------------------------------------------------------------------------

// Parse a Group from a stream.  A failure results in a ParseError, but leaves
// the original Group unchanged.
void Config::Group::parse_stream(std::istream& sin) {
    // Use a temporary map in case the parsing fails mid-stream.
    std::unordered_map<std::string,Config::Value> temp_map;
    std::string line;
    // Loop over each line of the stream
    while(std::getline(sin,line)) {
        Config::string_it key_start = line.begin();
        const Config::string_it end = line.end();
        // Strip leading whitespace
        consume_whitespace(key_start, end);
        if (key_start == end || *key_start == '#') {
            // If the line is empty or is comment-only, skip it
            continue;
        }
        // Find the end of the key
        auto key_stop = key_start;
        consume_key(key_stop, end);
        if (key_stop == key_start) {
            throw ParseError("Malformed line (empty key): " + line);
        }
        if (key_stop == end) {
            throw ParseError("Malformed line (key only): " + line);
        }
        // Check for equal sign (and strip surrounding whitespace)
        auto value_start = key_stop;
        consume_whitespace(value_start, end);
        if (*value_start != '=') {
            throw ParseError("Malformed line (no key-value separator): "
                    + line);
        }
        value_start++;
        consume_whitespace(value_start, end);
        // Split into key and value
        std::string key(key_start, key_stop);
        Config::Value value(std::string(value_start, end));
        // Load into the map
        if (temp_map.find(key) != temp_map.end()) {
            throw ParseError("Key \"" + key + "\" multiply defined.");
        }
        temp_map.emplace(key, value);
    }
    data_map = temp_map;
}

// ----------------------------------------------------------------------------

// Return the set of keys in the Group
std::vector<std::string> Config::Group::keys() const {
    std::vector<std::string> v;
    for (auto it = data_map.begin(); it != data_map.end(); it++) {
        v.push_back(it->first);
    }
    return v;
}

// ----------------------------------------------------------------------------

// Access a Value according to its key within the Group
Config::Value& Config::Group::operator[] (std::string key) {
    return data_map.at(key);
}

// ----------------------------------------------------------------------------

// Convert the Group to a std::string as if writing a new config file
std::string Config::Group::serialize() const {
    std::stringstream ss("");
    for (auto it = data_map.begin(); it != data_map.end(); it++) {
        ss << it->first << " = " << it->second << std::endl;
    }
    return ss.str();
}

// ----------------------------------------------------------------------------

// Write a Group to a stream
std::ostream& Config::operator<< (std::ostream& sout, const Config::Group& g) {
    sout << g.serialize();
    return sout;
}

