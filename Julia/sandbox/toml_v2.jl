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

    # eat whitespace at start of string and return what's left
    function consume_whitespace(s::String)
        idx = 1
        while idx <= length(s) && (s[idx] == ' ' || s[idx] == '\t')
            idx += 1
        end
        return(s[idx:end])
    end # function consume_whitespace

    # =========================================================================

    # eat the specified character or throw an error
    function consume_character(c::Char, s::String)
        if s[1] == c
            return(s[2:end])
        else
            throw(ParserError(join(["Expected \"", c, "\" at start of \"", s,
                "\"."])))
        end
    end

    # =========================================================================

    function consume_to_eol(s::String)
        remain = consume_whitespace(s)
        if length(remain) != 0 && remain[1] != '#'
            throw(ParserError(join(
                ["Non-comment characters remaining in line \"", s, "\"."])))
        end
    end

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
        num_str = split(s)[1]
        remain  = s[length(num_str)+1:end]
        if (contains(num_str, ".") || contains(num_str, "e") ||
            contains(num_str, "E"))
            try
                return(parsefloat(Float64, num_str), remain)
            catch
                throw(ParserError("Unable to parse as a float."))
            end
        else
            try
                return(parseint(Int64, num_str), remain)
            catch
                throw(ParserError("Unable to parse as an integer."))
            end
        end
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

    # Return a value based on parsing the String
    function analyze_value(s::String)
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
            value, s = parse_string_value(s)
        elseif s[1] == 't' || s[1] == 'f'
            # This is either a Boolean or nothing
            value, s = parse_boolean_value(s)
        elseif s[1] == '-' || s[1] == '+' || s[1] == '.' || is_digit(s[1])
            # This is either an Integer, a Float, both, or nothing
            value, s = parse_number_value(s)
        else
            # This is nothing
            throw(ParserError(join(["Unable to parse \"", s,
                "\" to a value."])))
        end

        # Return the remainder of the string
        return value, s
    end # function analyze_value

    # =========================================================================

    function serialize(v::UTF8String)
        output = ["\""]
        idx = 1
        while idx <= length(v)
            if v[idx] == '"'
                push!(output, "\\\"")
            elseif v[idx] == '\\'
                push!(output, "\\\\")
            elseif v[idx] == '\b'
                push!(output, "\\b")
            elseif v[idx] == '\t'
                push!(output, "\\t")
            elseif v[idx] == '\n'
                push!(output, "\\\n")
            elseif v[idx] == '\f'
                push!(output, "\\f")
            elseif v[idx] == '\r'
                push!(output, "\\r")
            else
                push!(output, v[idx])
            end
            idx += 1
        end
        pushd!(output, "\"")
        return(convert(UTF8String, join(output)))
    end # function serialize(UTF8String)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    function serialize(v::Bool)
        return(convert(UTF8String, string(v)))
    end # function serialize(Bool)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    function serialize(v::Int64)
        return(convert(UTF8String, string(v)))
    end # function serialize(Int64)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    function serialize(v::Float64)
        return(convert(UTF8String, string(v)))
    end # function serialize(Float64)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    function serialize(v)
        throw(ValueError("Value cannot be serialized."))
    end # function serialize(Any)

    # -------------------------------------------------------------------------
    # =========================================================================
    # -------------------------------------------------------------------------

    type Table
        data_map::Dict{UTF8String,Any}
    end

    function Table()
        return Table(Dict{UTF8String,Any}())
    end function # Table

    # =========================================================================

    function empty!(t::Table)
        empty!(t.data_map)
    end

    # =========================================================================

    function get_table(t::Table, key::String)
        if has(t, key)
            v = t.data_map[key]
            if isa(v, Table)
                return(v)
            else
                throw(TableError("Requested key is not a table."))
            end
        else
            throw(TableError("Requested key does not exist in table."))
        end
    end

    # =========================================================================

    function has(t::Table, key::String)
        return(key in keys(t.data_map))
    end

    # =========================================================================

    function has(t::Table, path::Array{String,1})
        current_table = t
        for p in path[1:length(path)-1]
            try
                current_table = get_table(current_table, p)
            catch e
                isa(e, TableError) || rethrow(e)
                return(false)
            end
        end
        return(has(current_table, path[length(path)]))
    end

    # =========================================================================

    function analyze_quoted_key(s::String)
        # Quoted keys follow the same rules as string values, so we can just
        # analyze it as a value and return the string if a value string results
        try
            str, s = parse_string_value(s)
        catch e
            isa(e, ParserError) || rethrow(e)
            throw(ParserError("Could not parse quoted key."))
        end
        if length(str) == 0
            throw(ParserError("Cannot have an empty quoted key."))
        end
        return(str, s)
    end

    # =========================================================================

    function analyze_bare_key(s::String)
        key = String[]
        idx = 1
        while (length(s) != 0 && (is_digit(s[idx]) || is_letter(s[idx]) ||
            s[idx] == '_' || s[idx] == '-'))
            push!(key, s[idx])
            idx += 1
        end
        if length(key) == 0
            throw(ParserError("Empty bare key."))
        end
        return(join(key), s[idx:end])
    end

    # =========================================================================

    function analyze_key(s::String)
        if s[1] == '"'
            return(analyze_quoted_key(s))
        else
            return(analyze_bare_key(s))
        end
    end

    # =========================================================================

    function analyze_table_name(s::String)
        tname = consume_whitespace(s)
        p, tname = analyze_key(tname)
        path = [p]
        tname = consume_whitespace(tname)
        while tname[1] == '.'
            tname = consume_character('.', tname)
            tname = consume_whitespace(tname)
            p, tname = analyze_key(tname)
            push!(path, p)
            tname = consume_whitespace(tname)
        end
        return(path)
    end

    # =========================================================================

    function parse_string_table(root::Table, s::String)
        empty!(root)
        current_Table = root
        lines = split(s, "\n")
        try
            for line in lines
                # Strip leading whitespace
                line = consume_whitespace(line)
                # What kind of line is it?
                if length(line) == 0 || line[1] == '#'
                    # If the line is empty or comment-only, skip it
                    continue
                elseif line[1] == '['
                    # This is the start of a new Table
                    # Note: All paths in a file will be specified from the root
                    line = consume_character('[', line)
                    path, line = analyze_table_name(line)
                    line = consume_character(']', line)
                    consume_to_eol(line)
                    # The TOML standard does not allow re-entering a Table
                    # after you've already created it and then moved to another
                    # Table.  Thus we generate an error if the Table already
                    # exists.
                    # TODO -- The TOML standard actually allows a slightly more
                    #         complex behavior: If you define Table [a.b], you
                    #         can then go back and fill in Table [a] so long as
                    #         [a] only exists because you built it as an
                    #         intermediary to build [a.b].  Thus I will need a
                    #         more complex bookkeeping mechanism to specify
                    #         whether a Table exists because it was directly
                    #         defined or because it was built as an
                    #         intermediary.
                    if has(root, path)
                        message = ["Key \"", path[1]]
                        for p in path[2:end]
                            push!(message, ".")
                            push!(message, p)
                        end
                        push!(message, "\" is not unique.")
                        throw(ParserError(join(message)))
                    end
                    # Create the Table (and all intermediaries)
                    current_table = get_table(root, path, true)
                else
                    # This is a key pair
                    key, line = analyze_key(line)
                    if has(current_table, key)
                        throw(ParserError(
                            join(["Key \"", key, "\" is not unique."])))
                    end
                    line = consume_whitespace(line)
                    line = consume_character("=", line)
                    line = consume_whitespace(line)
                    if line[1] == '['
                        # This is an array
                        arr = []
                        line = consume_character('[', line)
                        line = consume_whitespace(line)
                        while line[1] != ']'
                            value, line = parse_string_value(line)
                            if length(arr) == 0
                                arr = [value]
                            else
                                if typeof(arr[1]) == typeof(value)
                                    push!(arr, value)
                                else
                                    throw(ParserError(
                                        "Mixed-type arrays not allowed."))
                                end
                            end
                            line = consume_whitespace(line)
                            if line[1] == ','
                                line = consume_character(",", line)
                                line = consume_whitespace(line)
                            elseif line[1] != ']'
                                throw(ParserError(
                                    "Malformed array of values."))
                            end
                            end
                            line = consume_character("]", line)
                            consume_to_eol(line)
                            add(current_table, key, arr)
                        else
                            # This is a value
                            value, line = parse_string_value(line)
                            consume_to_eol(line)
                            add(current_table, key, value)
                        end
                    end
                end # "what kind of line" if-else ladder
            end # loop over lines
        catch e
            if isa(e, ParserError)
                empty!(root)
            end
            rethrow(e)
        end
    end # function parse_string_table
end # module TOML
