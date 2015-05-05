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

    // A typedef that will be used a lot internally
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
            void analyze(const std::string input_string);

        public:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
            friend std::ostream& operator<< (
                    std::ostream& sout, const Value& v);
    };

    // ========================================================================

    class Group {
        private:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Internal storage

            // The map that stores the key-value pairs
            std::unordered_map<std::string,Value> data_map;

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Private functions

            // Parsing
            // TODO -- better method of getting valid keys
            static void consume_key(string_it& it, const string_it& end);

        public:
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Public functions

            // Parsing
            void parse_string(const std::string s);
            void parse_file(const std::string filename);
            void parse_stream(std::istream& sin);

            // Get the list of keys
            std::vector<std::string> keys() const;

            // Access a Value by its key
            Value& operator[] (std::string key);

            // Output
            std::string serialize() const;
            friend std::ostream& operator<< (
                    std::ostream& sout, const Group& g);
    };

}

#endif // #ifndef CONFIG_H

