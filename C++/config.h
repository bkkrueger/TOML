/* A simple configuration file parser. */

#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Config {

    // ========================================================================

    // Typedefs in case the internal storage type ever needs to change.  Also
    // provides typedefs for the user (e.g. most users will use int, which is
    // not necessarily the same as int64_t, and using these typedefs will avoid
    // such an issue).
    // -- Note that there is a distinction between String (the type we use to
    //    store Values that can be read as strings) and std::string.  Yes, they
    //    are in fact defined to be the same, but conceptually they are
    //    different.  Many of the parsing routines for Value will use
    //    std::string, because they assume that the user will pass in a
    //    std::string.  That is not going to change if the internal data type
    //    used for String changes, hence the typedef is not used in those
    //    situations.
    typedef std::string String;
    typedef int64_t Integer;
    typedef double Float;
    typedef bool Boolean;

    // ========================================================================

    class ParseError : public std::runtime_error {
        public:
            ParseError(std::string msg): std::runtime_error(msg) {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class TypeError : public std::runtime_error {
        public:
            TypeError(std::string msg): std::runtime_error(msg) {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class ValueError : public std::runtime_error {
        public:
            ValueError(std::string msg): std::runtime_error(msg) {}
    };

    // ========================================================================

    class Value {
        private:
            // ----------------------------------------------------------------
            // Internal storage

            // The value in different formats
            String value_as_string;
            Integer value_as_integer;
            Float value_as_float;
            Boolean value_as_boolean;

            // Is the value available in the different formats?
            bool is_conformable_to_string;
            bool is_conformable_to_integer;
            bool is_conformable_to_float;
            bool is_conformable_to_boolean;

            // A typedef that will be used a lot internally
            typedef std::string::const_iterator string_it;

            // ----------------------------------------------------------------
            // Private functions

            // Parsing
            void parse_string(string_it& it, const string_it end);
            void parse_number(string_it& it, const string_it end);
                    // TODO const std::string::const_iterator end);
            void parse_boolean(std::string::const_iterator& it,
                    const std::string::const_iterator end);
            void analyze(const std::string input_string);

        public:
            // ----------------------------------------------------------------
            // Public functions

            // Constructors
            Value();
            Value(const std::string input_string);

            // Setters
            void set_from_string(const std::string input_string);
            void set(const String s);
            void set(const Integer i);
            void set(const Float d);
            void set(const Boolean b);

            // Getters
            String as_string() const;
            Integer as_integer() const;
            Float as_float() const;
            Boolean as_boolean() const;

            // Output
            std::string serialize() const;
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

