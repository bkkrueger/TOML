/* A simple configuration file parser. */

#ifndef CONFIG_H
#define CONFIG_H

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

    // The parsing routines, especially for Value, return the value in the
    // desired format.  Because numbers can be either Integers, Floats, or
    // both, we use this struct for returning Numbers.
    typedef struct {
        Integer integer_value;
        Float float_value;
        bool valid_integer;
        bool valid_float;
    } Number;

    // Some typedefs that will be used a lot internally
    typedef std::string::const_iterator string_it;

    // ========================================================================

    // All errors used here inherit from ConfigError (for inheritance and
    // catching).
    class Error : public std::runtime_error {
        public:
            Error(std::string msg): std::runtime_error(msg) {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class ParseError : public Error {
        public:
            ParseError(std::string msg): Error(msg) {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class TypeError : public Error {
        public:
            TypeError(std::string msg): Error(msg) {}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    class ValueError : public Error {
        public:
            ValueError(std::string msg): Error(msg) {}
    };

    // ========================================================================

    class Value {
        private:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Private functions

            // Parsing
            void clear();
            String parse_string(string_it& it, const string_it& end);
            Number parse_number(string_it& it, const string_it& end);
            Boolean parse_boolean(string_it& it, const string_it& end);

        public:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Public functions

            // Constructors
            Value();
            Value(string_it& it, const string_it& end);
            Value(const std::string input_string);

            // Setters
            void analyze(string_it& it, const string_it& end);
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

            // Type
            bool is_valid_string() const;
            bool is_valid_integer() const;
            bool is_valid_float() const;
            bool is_valid_boolean() const;

            // Output
            std::string serialize() const;
            friend std::ostream& operator<< (
                    std::ostream& sout, const Value& v);
    };

    // ========================================================================

    class ValueArray {
        private:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Internal storage

            // Vector to hold all the Values
            std::vector<Value> array;

            // Are the values available in the different formats?
            bool is_conformable_to_string;
            bool is_conformable_to_integer;
            bool is_conformable_to_float;
            bool is_conformable_to_boolean;

        public:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Public functions

            // Constructors
            ValueArray();

            // Size of the array
            unsigned size() const;

            // Add an element
            void add(const Value v);

            // Remove an element
            void remove(const unsigned index);
            void clear();

            // Access an element
            Value at(const unsigned index) const;

            // Convert to vector of a specific type
            std::vector<String> as_string() const;
            std::vector<Integer> as_integer() const;
            std::vector<Float> as_float() const;
            std::vector<Boolean> as_boolean() const;

            // Output
            std::string serialize() const;
            friend std::ostream& operator<< (
                    std::ostream& sout, const ValueArray& v);
    };

    // ========================================================================

    class Table {
        private:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Internal storage

            // The map for (key, value) pairs
            std::unordered_map<std::string,Value> scalar_map;
            // The map for (key, value array) pairs
            std::unordered_map<std::string,ValueArray> array_map;
            // The map for (key, table) pairs
            std::unordered_map<std::string,Table> table_map;

        public:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Public storage

            static const char comment = '#';

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Public functions

            // Parsing
            void parse_string(const std::string s);
            void parse_file(const std::string filename);
            void parse_stream(std::istream& sin);

            // Add an element
            void add(std::string key, Value& v);
            void add(std::string key, ValueArray& va);
            void add(std::string key, Table& t);

            // Get the list of keys
            std::vector<std::string> all_keys() const;
            std::vector<std::string> scalar_keys() const;
            std::vector<std::string> array_keys() const;
            std::vector<std::string> table_keys() const;

            // Does the key exist in the table?
            // TODO -- should be recursive or not?
            // TODO -- adding should not be recursive so that a file cannot
            //         have a key pair like table.subtable.key = "value"
            bool has(const std::string path) const;
            bool has(const std::vector<std::string> path) const;
            // TODO -- Should I have a method to tell whether a key is a Value,
            //         ValueArray, or Table?

            // Access an element by its key
            // TODO -- The get_table method will recurse down into the
            //         structure to get the appropriate element.  I should
            //         modify get_scalar and get_array to do the same (they
            //         will assume that the last element in the path is the
            //         name of the scalar or array they want).
            Value& get_scalar(const std::string key);
            ValueArray& get_array(const std::string key);
            Table& get_table(const std::string path, const bool create=true);
            Table& get_table(
                    const std::vector<std::string> path,
                    const bool create=true);

            // Output
            std::string serialize() const;
            friend std::ostream& operator<< (
                    std::ostream& sout, const Table& g);
    };

}

#endif // #ifndef CONFIG_H

