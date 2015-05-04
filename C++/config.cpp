/* A simple configuration file parser. */

#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================

typedef std::string::const_iterator string_it;

// ============================================================================

class TypeError : public std::runtime_error {
    public:
        TypeError(std::string msg): std::runtime_error(msg) {}
};

// ============================================================================

class ParseError : public std::runtime_error {
    public:
        ParseError(std::string msg): std::runtime_error(msg) {}
};

// ============================================================================

class ValueError : public std::runtime_error {
    public:
        ValueError(std::string msg): std::runtime_error(msg) {}
};

// ============================================================================

void consume_whitespace(string_it& it, const string_it& end) {
    while (it != end && (*it == ' ' || *it == '\t')) {
        it++;
    }
}

// ============================================================================

class Value {

    private:

        // --------------------------------------------------------------------

        std::string value_as_string;
        int64_t value_as_integer;
        double value_as_float;
        bool value_as_boolean;

        bool is_conformable_to_string;
        bool is_conformable_to_integer;
        bool is_conformable_to_float;
        bool is_conformable_to_boolean;

        // --------------------------------------------------------------------

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

        // --------------------------------------------------------------------

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
                    throw ParseError(message);
            }
        }

        // --------------------------------------------------------------------

        /*// Advance the iterator while it points at whitespace (space or tab)
        // and is not yet at end.
        void consume_whitespace(string_it& it, const string_it& end) const {
            while (it != end && (*it == ' ' || *it == '\t')) {
                it++;
            }
        }*/

        // --------------------------------------------------------------------

        // Attempt to parse the value as a string.  If it cannot be converted
        // to a valid string value, raise a ParseError.
        void parse_string(string_it& it, const string_it& end) {
            if (*it != '"') {
                throw ParseError("Unable to parse as a string.");
            }
            it++;
            std::string temp_string = "";
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
                        throw ParseError(message);
                    }
                } else if (*it == '"') {
                    break;
                } else {
                    temp_string += *it;
                    it++;
                }
            }
            if (*it != '"') {
                throw ParseError("Unable to parse as a string.");
            }
            it++;
            consume_whitespace(it, end);
            if (it != end && *it != '#') {
                throw ParseError("Trailing characters after string.");
            }
            value_as_string = temp_string;
            is_conformable_to_string = true;
        }

        // --------------------------------------------------------------------

        // Attempt to parse the value as a boolean .  If it cannot be converted
        // to a valid boolean, raise a ParseError.
        void parse_boolean(string_it& it, const string_it& end) {
            bool temp_bool;
            if (std::string(it, it+4) == "true") {
                it = it + 4;
                temp_bool = true;
            } else if (std::string(it, it+5) == "false") {
                it = it + 5;
                temp_bool = false;
            } else {
                throw ParseError("Unable to parse as a boolean.");
            }
            consume_whitespace(it, end);
            if (it != end && *it != '#') {
                throw ParseError("Trailing characters after boolean.");
            }
            value_as_boolean = temp_bool;
            is_conformable_to_boolean = true;
        }

        // --------------------------------------------------------------------

        // Attempt to parse the value as a number.  If it cannot be converted
        // to a valid number, raise a ParseError.
        // TODO -- handle underscore separators
        void parse_number(string_it& it, const string_it& end) {
            // sign
            int64_t sign;
            if (*it == '-') {
                sign = -1;
                it++;
            } else if (*it == '+') {
                sign = 1;
                it++;
            } else if (*it == '.' || is_digit(*it)) {
                sign = 1;
            } else {
                throw ParseError("Unable to parse as a number.");
            }
            // integer part
            int64_t ipart = 0;
            while (it != end && is_digit(*it)) {
                ipart = 10 * ipart + to_digit(*it);
                it++;
            }
            // decimal
            double dpart = 0;
            double shift = 0.1;
            if (it != end && *it == '.') {
                it++;
                while (it != end && is_digit(*it)) {
                    dpart += shift * to_digit(*it);
                    shift *= 0.1;
                    it++;
                }
            }
            // exponent (scientific notation)
            int64_t e_sign;
            int64_t exponent = 0;
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
                    throw ParseError("Invalid exponent in number.");
                }
                while (it != end && is_digit(*it)) {
                    exponent = 10 * exponent + to_digit(*it);
                    it++;
                }
                exponent *= e_sign;
            }
            // Trailing characters
            consume_whitespace(it, end);
            if (it != end && *it != '#') {
                throw ParseError("Trailing characters after number.");
            }
            // Construct the number
            if (dpart == 0 && exponent == 0) {
                // This is really an integer, and may also be a float
                int64_t as_integer = sign * ipart;
                double as_float = static_cast<double>(as_integer);
                if (as_integer == as_float) {
                    value_as_float = as_float;
                    is_conformable_to_float = true;
                }
                value_as_integer = sign * ipart;
                is_conformable_to_integer = true;
            } else {
                // This is really a float, and may also be an integer
                double as_float = sign * (static_cast<double>(ipart) + dpart) *
                    std::pow(10.0, exponent);
                int64_t as_integer = static_cast<int64_t>(as_float);
                if (as_float == as_integer) {
                    value_as_integer = as_integer;
                    is_conformable_to_integer = true;
                }
                value_as_float = as_float;
                is_conformable_to_float = true;
            }
        }

        // --------------------------------------------------------------------

        void analyze(const std::string input_string) {
            string_it it = input_string.begin();
            string_it end = input_string.end();

            // Trim leading whitespace
            consume_whitespace(it, end);

            // Ensure there is something (non-comment) left in the string
            if (it == end || *it == '#') {
                throw ParseError("Empty value.");
            }

            // Choose which type to parse
            if (*it == '"') {
                parse_string(it, end);
            } else if (*it == 't' || *it == 'f') {
                parse_boolean(it, end);
            } else if (*it == '-' || *it == '+' || *it == '.' ||
                    is_digit(*it)) {
                parse_number(it, end);
            } else {
                throw ParseError("Unable to parse \"" + input_string +
                        "\" to a value.");
            }
        }

        // --------------------------------------------------------------------

    protected:

        // --------------------------------------------------------------------

    public:

        // --------------------------------------------------------------------

        Value():
            value_as_string(""),
            is_conformable_to_string(false),
            value_as_integer(0),
            is_conformable_to_integer(false),
            value_as_float(0.0),
            is_conformable_to_float(false),
            value_as_boolean(false),
            is_conformable_to_boolean(false) {}

        // --------------------------------------------------------------------

        Value(const std::string input_string):
            value_as_string(""),
            is_conformable_to_string(false),
            value_as_integer(0),
            is_conformable_to_integer(false),
            value_as_float(0.0),
            is_conformable_to_float(false),
            value_as_boolean(false),
            is_conformable_to_boolean(false) {
            analyze(input_string);
        }

        // --------------------------------------------------------------------

        void set_from_string(const std::string input_string) {
            value_as_string = "";
            is_conformable_to_string = false;
            value_as_integer = 0;
            is_conformable_to_integer = false;
            value_as_float = 0.0;
            is_conformable_to_float = false;
            value_as_boolean = false;
            is_conformable_to_boolean = false;
            analyze(input_string);
        }

        // --------------------------------------------------------------------

        void set(const std::string s) {
            value_as_string = s;
            is_conformable_to_string = true;
            value_as_integer = 0;
            is_conformable_to_integer = false;
            value_as_float = 0.0;
            is_conformable_to_float = false;
            value_as_boolean = false;
            is_conformable_to_boolean = false;
        }

        // --------------------------------------------------------------------

        void set(const int64_t i) {
            value_as_string = "";
            is_conformable_to_string = false;
            value_as_integer = i;
            is_conformable_to_integer = true;
            value_as_float = static_cast<double>(i);
            is_conformable_to_float = true;
            value_as_boolean = false;
            is_conformable_to_boolean = false;
        }

        // --------------------------------------------------------------------

        void set(const double d) {
            value_as_string = "";
            is_conformable_to_string = false;
            if (std::abs(d) == d) {
                value_as_integer = d;
                is_conformable_to_integer = true;
            } else {
                value_as_integer = 0;
                is_conformable_to_integer = false;
            }
            value_as_float = d;
            is_conformable_to_float = true;
            value_as_boolean = false;
            is_conformable_to_boolean = false;
        }

        // --------------------------------------------------------------------

        void set(const bool b) {
            value_as_string = "";
            is_conformable_to_string = false;
            value_as_integer = 0;
            is_conformable_to_integer = false;
            value_as_float = 0.0;
            is_conformable_to_float = false;
            value_as_boolean = b;
            is_conformable_to_boolean = true;
        }

        // --------------------------------------------------------------------

        std::string as_string() const {
            if (is_conformable_to_string) {
                return value_as_string;
            } else {
                throw TypeError("Value cannot be converted to a string.");
            }
        }

        // --------------------------------------------------------------------

        int64_t as_integer() const {
            if (is_conformable_to_integer) {
                return value_as_integer;
            } else {
                throw TypeError("Value cannot be converted to an integer.");
            }
        }

        // --------------------------------------------------------------------

        double as_float() const {
            if (is_conformable_to_float) {
                return value_as_float;
            } else {
                throw TypeError("Value cannot be converted to an integer.");
            }
        }

        // --------------------------------------------------------------------

        bool as_boolean() const {
            if (is_conformable_to_boolean) {
                return value_as_boolean;
            } else {
                throw TypeError("Value cannot be converted to an boolean.");
            }
        }

        // --------------------------------------------------------------------

        std::string serialize() const {
            if (is_conformable_to_boolean) {
                if (value_as_boolean) {
                    return "true";
                } else {
                    return "false";
                }
            } else if (is_conformable_to_integer) {
                std::stringstream ss;
                ss << value_as_integer;
                return ss.str();
            } else if (is_conformable_to_float) {
                std::stringstream ss;
                ss << std::setprecision(15) << value_as_float;
                return ss.str();
            } else if (is_conformable_to_string) {
                string_it it = value_as_string.begin();
                string_it end = value_as_string.end();
                std::string output = "\"";
                while (it != end) {
                    // TODO -- Check the escape list for TOML
                    if (*it == '"' || *it == '\\') {
                        output.append(1, '\\');
                    }
                    output.append(1, *it);
                    it++;
                }
                output += "\"";
                return output;
            } else {
                throw ValueError("Value cannot be serialized.");
            }
        }

};

std::ostream& operator<< (std::ostream& sout, Value v) {
    sout << v.serialize();
    return sout;
}

// ============================================================================

class Table {

    private:

        std::unordered_map<std::string,Value> data_map;

        // --------------------------------------------------------------------

        // TODO -- better method of getting valid keys
        void consume_key(string_it& it, const string_it& end) {
            while (it != end && (*it != ' ' && *it != '\t' && *it != '=')) {
                it++;
            }
        }

        // --------------------------------------------------------------------

    public:

        // --------------------------------------------------------------------

        void parse_string(const std::string s) {
            std::istringstream iss(s);
            parse_stream(iss);
        }

        // --------------------------------------------------------------------

        void parse_file(const std::string filename) {
            std::ifstream fin;
            fin.open(filename);
            parse_stream(fin);
            fin.close();
        }

        // --------------------------------------------------------------------

        void parse_stream(std::istream& sin) {
            std::unordered_map<std::string,Value> temp_map;
            std::string line;
            while(std::getline(sin,line)) {
                string_it key_start = line.begin();
                string_it end = line.end();
                // Strip leading whitespace
                consume_whitespace(key_start, end);
                if (key_start == end || *key_start == '#') {
                    continue;
                }
                // Find the end of the key
                auto key_stop = key_start;
                consume_key(key_stop, end);
                if (key_stop == key_start) {
                    throw ParseError("Key is empty: " + line);
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
                Value value(std::string(value_start,end));
                // Load into the map
                if (temp_map.find(key) != temp_map.end()) {
                    throw ParseError("Key \"" + key + "\" multiply defined.");
                }
                temp_map.emplace(key, value);
            }
            data_map = temp_map;
        }

        // --------------------------------------------------------------------

        std::vector<std::string> keys() const {
            std::vector<std::string> v;
            for (auto it = data_map.begin(); it != data_map.end(); it++) {
                v.push_back(it->first);
            }
            return v;
        }

        // --------------------------------------------------------------------

        Value& operator[] (std::string key) {
            return data_map.at(key);
        }

};

