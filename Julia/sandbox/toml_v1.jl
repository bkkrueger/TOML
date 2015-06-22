module TOML

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    # All TOML errors inherit from the abstract Error
    abstract Error <: Exception

    # Errors that arise in parsing input
    type ParserError <: Error
        message::String
    end
    Base.showerror(io::IO, e::ParserError) = print(io,
        join(["TOML.ParserError: ", e.message]))

    # Errors that arise in Table structure
    type TableError <: Error
        message::String
    end
    Base.showerror(io::IO, e::TableError) = print(io,
        join(["TOML.TableError: ", e.message]))

    # Errors that arise in handling types within TOML
    type TypeConversionError <: Error
        message::String
    end
    Base.showerror(io::IO, e::TypeConversionError) = print(io,
        join(["TOML.TypeConversionError: ", e.message]))

    # Errors that arise in dealing with Values
    type ValueError <: Error
        message::String
    end
    Base.showerror(io::IO, e::ValueError) = print(io,
        join(["TOML.ValueError: ", e.message]))

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    # The parsing routines, especially for Value, return the value in the
    # desired format.  However, numbers can be either integers, floats, or
    # both; thus we use this struct for returning numbers.
    type NumberValue
        integer_value::Int64
        float_value::Float64
        valid_integer::Bool
        valid_float::Bool
    end # type NumberValue

    function NumberValue(; integer_value::Int64 = 0,
        float_value::Float64 = 0.0, valid_integer::Bool = false,
        valid_float::Bool = false)
        return(NumberValue(integer_value, float_value,
            valid_integer, valid_float))
    end

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    # Check if the character is a valid letter (a-zA-Z).
    function is_letter(c::Char)
        if c == 'a' || c == 'A'
            return(true)
        elseif c == 'b' || c == 'B'
            return(true)
        elseif c == 'c' || c == 'C'
            return(true)
        elseif c == 'd' || c == 'D'
            return(true)
        elseif c == 'e' || c == 'E'
            return(true)
        elseif c == 'f' || c == 'F'
            return(true)
        elseif c == 'g' || c == 'G'
            return(true)
        elseif c == 'h' || c == 'H'
            return(true)
        elseif c == 'i' || c == 'I'
            return(true)
        elseif c == 'j' || c == 'J'
            return(true)
        elseif c == 'k' || c == 'K'
            return(true)
        elseif c == 'l' || c == 'L'
            return(true)
        elseif c == 'm' || c == 'M'
            return(true)
        elseif c == 'n' || c == 'N'
            return(true)
        elseif c == 'o' || c == 'O'
            return(true)
        elseif c == 'p' || c == 'P'
            return(true)
        elseif c == 'q' || c == 'Q'
            return(true)
        elseif c == 'r' || c == 'R'
            return(true)
        elseif c == 's' || c == 'S'
            return(true)
        elseif c == 't' || c == 'T'
            return(true)
        elseif c == 'u' || c == 'U'
            return(true)
        elseif c == 'v' || c == 'V'
            return(true)
        elseif c == 'w' || c == 'W'
            return(true)
        elseif c == 'x' || c == 'X'
            return(true)
        elseif c == 'y' || c == 'Y'
            return(true)
        elseif c == 'z' || c == 'Z'
            return(true)
        else
            return(false)
        end
    end # function is_letter

    # =========================================================================

    # Check if the character is a valid digit (0-9).
    function is_digit(c::Char)
        if c == '0'
            return(true)
        elseif c == '1'
            return(true)
        elseif c == '2'
            return(true)
        elseif c == '3'
            return(true)
        elseif c == '4'
            return(true)
        elseif c == '5'
            return(true)
        elseif c == '6'
            return(true)
        elseif c == '7'
            return(true)
        elseif c == '8'
            return(true)
        elseif c == '9'
            return(true)
        else
            return(false)
        end
    end # function is_digit

    # =========================================================================

    # Convert valid digit characters to the correct number
    function to_digit(c::Char)
        if c == '0'
            return(0::Int64)
        elseif c == '1'
            return(1::Int64)
        elseif c == '2'
            return(2::Int64)
        elseif c == '3'
            return(3::Int64)
        elseif c == '4'
            return(4::Int64)
        elseif c == '5'
            return(5::Int64)
        elseif c == '6'
            return(6::Int64)
        elseif c == '7'
            return(7::Int64)
        elseif c == '8'
            return(8::Int64)
        elseif c == '9'
            return(9::Int64)
        else
            throw(ParserError(join(["Character \"", string(c),
                "\" is not a digit."])))
        end
    end # function to_digit

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    # skip ahead to end of whitespace -- return number of whitespace characters
    function consume_whitespace(s::String)
        idx = 1
        while idx <= length(s) && (s[idx] == ' ' || s[idx] == '\t')
            idx += 1
        end
        return(s[idx:end])
    end # function consume_whitespace

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    # Attempt to parse a string into a string Value (which follows a particular
    # format).  Return the string and the trailing characters.  Throw a
    # ParserError if parsing fails.
    function parse_string_value(s::String)
        idx = 1
        if s[idx] != '"'
            throw(ParserError("Unable to parse as a string."))
        end
        idx += 1
        temp_str::UTF8String = ""
        while idx <= length(s)
            if s[idx] == '\\'
                idx += 1
                if s[idx] == '"'
                    temp_str = join([temp_str, '"'])
                    idx += 1
                elseif s[idx] == '\\'
                    temp_str = join([temp_str, '\\'])
                    idx += 1
                elseif s[idx] == 'b'
                    temp_str = join([temp_str, "\b"])
                    idx += 1
                elseif s[idx] == 't'
                    temp_str = join([temp_str, "\t"])
                    idx += 1
                elseif s[idx] == 'n'
                    temp_str = join([temp_str, "\n"])
                    idx += 1
                elseif s[idx] == 'f'
                    temp_str = join([temp_str, "\f"])
                    idx += 1
                elseif s[idx] == 'r'
                    temp_str = join([temp_str, "\r"])
                    idx += 1
                else
                    throw(ParserError(join(["Unknown escale character \"\\",
                        s[idx], "\"."])))
                end
            elseif s[idx] == '"'
                break
            else
                temp_str = join([temp_str, s[idx]])
                idx += 1
            end
        end
        if s[idx] != '"'
            throw(ParserError("Unable to parse as a string."))
        end
        idx += 1
        return(temp_str, s[idx:end])
    end # function parse_string_value

    # =========================================================================

    # Attempt to parse a string as a number.  Return the NumberValue struct or
    # raise a ParserError.
    function parse_number_value(s::String)
        temp_num::NumberValue = NumberValue()
        temp_num.valid_integer = false
        temp_num.integer_value = 0
        temp_num.valid_float = false
        temp_num.float_value = 0.0
        idx = 1
        # sign
        sign::Int64
        if s[idx] == '-'
            sign = -1
            idx += 1
        elseif s[idx] == '+'
            sign = 1
            idx += 1
        elseif s[idx] == '.' || is_digit(s[idx])
            sign = 1
        else
            throw(ParserError("Unable to parse as a number."))
        end
        # integer part
        ipart::Int64 = 0
        while idx <= length(s) && is_digit(s[idx])
            next_ipart = 10 * ipart + to_digit(s[idx])
            idx += 1
            if next_ipart <= ipart
                throw(ParserError("Unable to parse as a number."))
            end
            ipart = next_ipart
        end
        # decimal
        dpart::Float64 = 0.0
        shift::Float64 = 0.1
        if idx <= length(s) && s[idx] == '.'
            idx += 1
            while idx <= length(s) && is_digit(s[idx])
                dpart += shift * to_digit(s[idx])
                shift *= 0.1
                idx += 1
            end
        end
        # exponent (scientific notation)
        e_sign::Int64
        exponent::Int64 = 0
        if idx <= length(s) && (s[idx] == 'e' || s[idx] == 'E')
            idx += 1
            if s[idx] == '-'
                e_sign = -1
                idx += 1
            elseif s[idx] == '+'
                e_sign = 1
                idx += 1
            elseif is_digit(s[idx])
                e_sign = 1
            else
                throw(ParserError("Invalid exponent in number."))
            end
            while idx <= length(s) && is_digit(s[idx])
                exponent = 10 * exponent + to_digit(s[idx])
                idx += 1
            end
            exponent *= e_sign
        end
        # construct the number
        as_integer::Int64 = 0
        as_float::Float64 = 0.0
        if dpart == 0 && exponent == 0
            # This is really an integer, and may also be a float
            as_integer = sign * ipart
            as_float = 0.0
            try
                as_float = convert(Float64, as_integer)
                back_to_integer::Int64 = convert(Int64, as_float)
                if back_to_integer == as_integer
                    temp_num.float_value = as_float
                    temp_num.valid_float = true
                end
            catch e
                isa(e, InexactError) || rethrow()
            end
            temp_num.integer_value = as_integer
            temp_num.valid_integer = true
        else
            # This is really a float, and may also be an integer
            as_float = sign * (convert(Float64, ipart) + dpart) * 10.0^exponent
            as_integer = 0
            try
                as_integer = convert(Int64, as_float)
                back_to_float::Float64 = convert(Float64, as_integer)
                if back_to_float == as_float
                    temp_num.integer_value = as_integer
                    temp_num.valid_integer = true
                end
            catch e
                isa(e, InexactError) || rethrow()
            end
            temp_num.float_value = as_float
            temp_num.valid_float = true
        end
        return(temp_num, s[idx:end])
    end # function parse_number_value

    # =========================================================================

    # Attempt to parse a string as a boolean.  Return the boolean or raise a
    # ParserError.
    function parse_boolean_value(s::String)
        temp_bool::Bool = false
        if length(s) >= 4 && s[1:4] == "true"
            idx = 5
            temp_bool = true
        elseif length(s) >= 5 && s[1:5] == "false"
            idx = 6
            temp_bool = false
        else
            throw(ParserError("Unable to parse as a boolean."))
        end
        return(temp_bool, s[idx:end])
    end # function parse_boolean_value

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    type Value
        # The value in different formats
        value_as_string::UTF8String
        value_as_integer::Int64
        value_as_float::Float64
        value_as_boolean::Bool
        # Is the value available in different formats?
        is_conformable_to_string::Bool
        is_conformable_to_integer::Bool
        is_conformable_to_float::Bool
        is_conformable_to_boolean::Bool
    end # type Value

    # =========================================================================

    function Value()
        return(Value("", 0, 0.0, false, false, false, false, false))
    end # function Value()

    # =========================================================================

    function clear(v::Value)
        # not conformable to anything
        v.is_conformable_to_string = false
        v.is_conformable_to_integer = false
        v.is_conformable_to_float = false
        v.is_conformable_to_boolean = false
        # wipe the value(s)
        v.value_as_string = ""
        v.value_as_integer = 0
        v.value_as_float = 0.0
        v.value_as_boolean = false
    end # function clear

    # =========================================================================

    # Set the Value based on parsing the String
    function analyze(v::Value, s::String)
        # Clear the current internal values and flags
        clear(v)

        # Trim leading whitespace
        s = consume_whitespace(s)

        # Ensure there is something (non-comment) left in the string
        if length(s) == 0 || s[1] == '#'
            throw(ParserError("Empty value."))
        end

        # Choose which type to parse
        if s[1] == '"'
            # This is either a String or nothing
            temp_string::UTF8String, s = parse_string_value(s)
            # Save it
            v.value_as_string = temp_string
            v.is_conformable_to_string = true
        elseif s[1] == 't' || s[1] == 'f'
            # This is either a Boolean or nothing
            temp_boolean::Bool, s = parse_boolean_value(s)
            # Save it
            v.value_as_boolean = temp_boolean
            is_conformable_to_boolean = true
        elseif s[1] == '-' || s[1] == '+' || s[1] == '.' || is_digit(s[1])
            # This is either an Integer, a Float, both, or nothing
            temp_number::NumberValue, s = parse_number_value(s)
            # Save it
            v.value_as_integer = temp_number.integer_value
            v.value_as_float = temp_number.float_value
            v.is_conformable_to_integer = temp_number.valid_integer
            v.is_conformable_to_float = temp_number.valid_float
        else
            # This is nothing
            throw(ParserError(join(["Unable to parse \"", s,
                "\" to a value."])))
        end

        # Return the remainder of the string
        return s
    end # function analyze

    # =========================================================================

    # Set the Value from a String
    function set(v::Value, s::UTF8String)
        clear(v)
        v.value_as_string = s
        v.is_conformable_to_string = true
    end # function set(Value, UTF8String)

    # =========================================================================

    # Set the Value from an Integer
    function set(v::Value, i::Int64)
        clear(v)
        v.value_as_integer = i
        v.is_conformable_to_integer = true
        as_float::Float64 = 0.0
        try
            as_float = convert(Float64, i)
            back_to_integer::Int64 = convert(Int64, as_float)
            if back_to_integer == i
                v.value_as_float = as_float
                v.is_conformable_to_float = true
            end
        catch e
            isa(e, InexactError) || rethrow()
        end
    end # function set(Value, Int64)

    # =========================================================================

    # Set the Value from a Float
    function set(v::Value, f::Float64)
        clear(v)
        v.value_as_flaot = f
        v.is_conformable_to_float = true
        as_integer::Int64 = 0
        try
            as_integer = convert(Int64, f)
            back_to_float::Float64 = convert(Float64, as_integer)
            if back_to_float == f
                v.value_as_integer = as_integer
                v.is_conformable_to_integer = true
            end
        catch e
            isa(e, InexactError) || rethrow()
        end
    end # function set(Value, Float)

    # =========================================================================

    # Set the Value from a Boolean
    function set(v::Value, b::Bool)
        clear(v)
        v.value_as_boolean = b
        v.is_conformable_to_boolean = true
    end # function set(Value, Boolean)

    # =========================================================================

    # Return the Value as a String
    function as_string(v::Value)
        if v.is_conformable_to_string
            return(v.value_as_string)
        else
            throw(TypeConversionError(
                "Value cannot be converted to a string."))
        end
    end # function as_string

    # =========================================================================

    # Return the Value as an Integer
    function as_integer(v::Value)
        if v.is_conformable_to_integer
            return(v.value_as_integer)
        else
            throw(TypeConversionError(
                "Value cannot be converted to an integer."))
        end
    end # function as_integer

    # =========================================================================

    # Return the Value as a Float
    function as_float(v::Value)
        if v.is_conformable_to_float
            return(v.value_as_float)
        else
            throw(TypeConversionError("Value cannot be converted to a float."))
        end
    end # function as_float

    # =========================================================================

    # Return the Value as a Boolean
    function as_boolean(v::Value)
        if v.is_conformable_to_boolean
            return(v.value_as_boolean)
        else
            throw(TypeConversionError(
                "Value cannot be converted to a boolean."))
        end
    end # function as_boolean

    # =========================================================================

    # Convert the Value to a string as if writing a new TOML file
    function serialize(v::Value)
        if v.is_conformable_to_boolean
            # Write as a Boolean
            if v.value_as_boolean
                return("true")
            else
                return("false")
            end
        elseif v.is_conformable_to_integer
            # Write as an Integer (anything conformable to both Integer and
            # Float will appear as an Integer because of the ordering here)
            return(string(v.value_as_integer))
        elseif v.is_conformable_to_float
            # Write as a Float
            return(string(v.value_as_float))
        elseif v.is_conformable_to_string
            idx = 1
            output = "\""
            while idx <= length(v.value_as_string)
                if v.value_as_string[idx] == '"'
                    output = join([output, "\\\""])
                elseif v.value_as_string[idx] == '\\'
                    output = join([output, "\\\\"])
                elseif v.value_as_string[idx] == '\b'
                    output = join([output, "\\\b"])
                elseif v.value_as_string[idx] == '\t'
                    output = join([output, "\\\t"])
                elseif v.value_as_string[idx] == '\n'
                    output = join([output, "\\\n"])
                elseif v.value_as_string[idx] == '\f'
                    output = join([output, "\\\f"])
                elseif v.value_as_string[idx] == '\r'
                    output = join([output, "\\\r"])
                else
                    output = join([output, v.value_as_string[idx]])
                end
                idx += 1
            end
            output = join([output, "\""])
            return(output)
        else
            # Not actually a valid Value
            throw(ValueError("Value cannot be serialized."))
        end
    end # function serialize

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    type ValueArray
        array::Array{Value,1}
        is_conformable_to_string::Bool
        is_conformable_to_integer::Bool
        is_conformable_to_float::Bool
        is_conformable_to_boolean::Bool
    end # type ValueArray

    # =========================================================================

    function ValueArray()
        return(ValueArray([], false, false, false, false))
    end # function ValueArray

    # =========================================================================

    function size(va::ValueArray)
        return(length(va.array))
    end # function size

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    function clear(va::ValueArray)
        while length(va.array) > 0
            deleteat!(va.array, 1)
        va.is_conformable_to_string = false
        va.is_conformable_to_integer = false
        va.is_conformable_to_float = false
        va.is_conformable_to_boolean = false
    end # function clear

    # =========================================================================

    function add(va::ValueArray, v::Value)
        if length(va) == 0
            push!(va, v)
            va.is_conformable_to_string = v.is_conformable_to_string
            va.is_conformable_to_integer = v.is_conformable_to_integer
            va.is_conformable_to_float = v.is_conformable_to_float
            va.is_conformable_to_boolean = v.is_conformable_to_boolean
        else
            if va.is_conformable_to_string && v.is_conformable_to_string
                push!(va, v)
            elseif va.is_conformable_to_integer && v.is_conformable_to_integer
                push!(va, v)
            elseif va.is_conformable_to_float && v.is_conformable_to_float
                push!(va, v)
            elseif va.is_conformable_to_boolean && v.is_conformable_to_boolean
                push!(va, v)
            else
                throw(ValueError(
                    "Value with invalid type cannot be added to ValueArray."))
            end
        end
    end # function add(ValueArray, Value)

    # =========================================================================

    function remove(va::ValueArray, index::Integer)
        # The deleteat! function already handles bounds checking for me
        deleteat!(va.array, index)
    end # function remove

    # =========================================================================

    function as_string(va::ValueArray)
        arr::Array{UTF8String,1} = []
        for index = 1:length(va)
            push!(arr, va[index])
        end
        return(arr)
    end # function as_string(ValueArray)

    # =========================================================================

end # module TOML
