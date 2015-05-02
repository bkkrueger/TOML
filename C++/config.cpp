/* A simple configuration file parser. */

#include <cmath>
#include <string>

// ============================================================================

typedef std::string::const_iterator string_it;

// ============================================================================

class ParseError : public std::runtime_error {
    public:
        ParseError(std::string msg): std::runtime_error(msg) {}
};

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

        static bool is_digit(const char c) const {
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

        static unsigned to_digit(const char c) const {
            switch(c) {
                case '0':
                    return 0;
                case '1':
                    return 1;
                case '2':
                    return 2;
                case '3':
                    return 3;
                case '4':
                    return 4;
                case '5':
                    return 5;
                case '6':
                    return 6;
                case '7':
                    return 7;
                case '8':
                    return 8;
                case '9':
                    return 9;
                default:
                    throw ParseError("Character \"" +c+ "\" is not a digit.");

        // --------------------------------------------------------------------

        // Advance the iterator while it points at whitespace (space or tab)
        // and is not yet at end.
        void consume_whitespace(string_it& it, const string_it& end) const {
            while (it != end && (*it == ' ' || *it == '\t')) {
                it++;
            }
        }

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
                if (*it == '\') {
                    it++;
                    if (*it == '"') {
                        temp_string += "\"";
                        it++;
                    } else if (*it == '\') {
                        temp_string += "\\";
                        it++;
                    } else {
                        // TODO -- Check the escape list for TOML
                        throw ParseError("Unknown escape character \"\\" +
                                *it + "\".");
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
            temp_bool;
            if (std::string(it, it+4) == "true") {
                it = it + 4;
                temp_bool = true;
            } else (std::string(it, it+5) == "false") {
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
        void parse_number(string_it& it, const string_it& end) {
            // sign
            int sign;
            if (*it == '-') {
                sign = -1;
                it++;
            } else if (*it == '+') {
                sign = 1;
                it++;
            } else if (is_digit(it)) {
                sign = 1;
            }
            // integer part
            int ipart = 0;
            while (it != end && is_digit(it)) {
                ipart = 10 * ipart + to_digit(*it);
                it++;
            }
            // decimal
            double dpart = 0;
            double shift = 0.1;
            if (it != end && *it == '.') {
                it++;
                while (it != end && is_digit(it)) {
                    dpart = shift * to_digit(*it);
                    shift *= 0.1;
                }
            }
            // exponent (scientific notation)
            double exponent = 0;
            if (it != end && (*it == 'e' || *it == 'E')) {
                it++;
                while (it != end && is_digit(it)) {
                    exponent = 10 * exponent + to_digit(*it);
                }
            }
            // Trailing characters
            consume_whitespace(it, end);
            if (it != end && *it != '#') {
                throw ParseError("Trailing characters after number.");
            }
            // Construct the number
            double abs_number = (ipart + dpart) * std::pow(10, exponent);
            if (dpart == 0 && exponent == 0) {
                value_as_number = sign * ipart;
                is_conformable_to_integer = true;
            } else if (std::floor(abs_number) == abs_number) {
                value_as_number = sign * abs_number;
                is_conformable_to_integer = true;
            }
            value_as_float = sign * abs_number;
            is_conformable_to_float = true;
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
                try {
                    parse_string(it, end);
                } catch(ParseError& pe) {
                    throw ParseError("Unable to parse \"" + input_string +
                            "\" to a value.");
                }
            } else if (*it == 't' || *it == 'f') {
                try {
                    parse_boolean(it, end);
                } catch(ParseError& pe) {
                    throw ParseError("Unable to parse \"" + input_string +
                            "\" to a value.");
                }
            } else if (*it == '-' || *it == '+' || *it == '.' ||
                    is_digit(*it)) {
                try {
                    parse_number(it, end);
                } catch(ParseError& pe) {
                    throw ParseError("Unable to parse \"" + input_string +
                            "\" to a value.");
                }
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
            value_as_integer(0),
            value_as_float(0.0),
            value_as_boolean(false),
            is_conformable_to_string(false),
            is_conformable_to_integer(false),
            is_conformable_to_float(false),
            is_conformable_to_boolean(false) { }

        // --------------------------------------------------------------------

        Value(const std::string input_string):
            value_as_string(""),
            value_as_integer(0),
            value_as_float(0.0),
            value_as_boolean(false),
            is_conformable_to_string(false),
            is_conformable_to_integer(false),
            is_conformable_to_float(false),
            is_conformable_to_boolean(false) {
            analyze(input_string);
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
            if (is_conformable_to_integer) {
                return value_as_integer;
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

};

