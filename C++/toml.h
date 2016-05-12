/* A simple configuration file parser.
 *
 * Over time this is intended to grow to match the TOML format.  Currently it
 * provides a subset of TOML.
 */

#ifndef TOML_H
#define TOML_H

#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/container/flat_map.hpp>
#include <vector>

namespace TOML {

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

    // All errors used here inherit from Error (for inheritance and
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

    class TableError : public Error {
        public:
            TableError(std::string msg): Error(msg) {}
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

            // Note:
            //     Originally this was developed using
            // std::unordered_map<std::string,Table>, which is fine in clang++
            // but fails in g++.  This is known as a recursive data type, with
            // Table having a container of Tables.  However the STL is not
            // required to accept incomplete types as templates (Table is an
            // incomplete type at that point because it has not been fully
            // created when I ask for a map that contains Tables), because that
            // can potentially cause problems in some cases.  It turns out that
            // clang++ is able to determine that Table is a recursive type and
            // manages to compile, while g++ does not allow this because it is
            // not part of the standard.  The Boost containers package is
            // designed so that most of its containers can be templated with
            // incomplete types in order to support recursive types.  Changing
            // from an unordered_map to a flat_map means I'm not using a
            // variation on an ordered map, but that's fine.  I don't expect
            // sorting to matter, or for the performance differences between
            // flat_map and map to matter, because of the small size of most
            // TOML files.
            //     If you don't want to use the Boost libraries (there are
            // reasons for avoiding libraries at times), you could alternately
            // use a std::unordered_map<std::string,Table*>, because then you
            // have pointers (probably a smart pointer is better than a bare
            // pointer, but I'll leave that to you).  But then you have to go
            // through the logic, switch things to use pointers, and make sure
            // you don't introduce memory leaks.  I was too lazy to that yet.

            // The map for (key, value) pairs
            boost::container::flat_map<std::string,Value> scalar_map;
            // The map for (key, value array) pairs
            boost::container::flat_map<std::string,ValueArray> array_map;
            // The map for (key, table) pairs
            boost::container::flat_map<std::string,Table> table_map;

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
            static bool valid_key(const std::string key);

            // Add an element
            void add(const std::string key, const Value& v);
            void add(const std::string key, const ValueArray& va);
            void add(const std::string key, const Table& t);

            // Get the list of keys
            std::vector<std::string> all_keys() const;
            std::vector<std::string> scalar_keys() const;
            std::vector<std::string> array_keys() const;
            std::vector<std::string> table_keys() const;

            // Does the key exist in the table?
            bool has(const std::string key) const;
            // This form allows you to specify a path (vector of keys to follow
            // in order to dive into nested tables), instead of having to
            // manually work through all path elements one at a time.
            bool has(const std::vector<std::string> path) const;
            bool has_scalar(const std::string key) const;
            bool has_array(const std::string key) const;
            bool has_table(const std::string key) const;

            // Access an element by its key
            Value& get_scalar(const std::string key);
            ValueArray& get_array(const std::string key);
            Table& get_table(const std::string key);
            const Table& get_table(const std::string key) const;
            // This form allows you to specify a path (vector of keys to follow
            // in order to dive into nested tables), instead of having to
            // manually work through all path elements one at a time.
            Table& get_table(const std::vector<std::string> path,
                    const bool create=false);
            const Table& get_table(const std::vector<std::string> path) const;

            // Clear the Table
            void clear();

            // Output
            std::string serialize(unsigned indent_level=0) const;
            friend std::ostream& operator<< (
                    std::ostream& sout, const Table& g);
    };

}

#endif // #ifndef TOML_H

