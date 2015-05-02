# config
A simple configuration format (hopefully will grow to match a standard such as
TOML), implemented in various languages.  Allows consistency across programs
written in different languages.

# Example File

```
# Comments start with a hash mark
string_variable = "This is a String.  Double quotes must be escaped, like this:
\".  Backslashes must also be escaped, like this: \\."

integer_variable = 1 # Comments are permitted at the end of a line

float_variable = 3.14

boolean_variable = false
# true or false; for now case-sensitive (for consistency with TOML), but I may
# decide to make booleans case-insensitive, because it's so irritating to have
# to remember true/false (C++), True/False (Python), etc (not to mention
# .true./.false. in Fortran).
```

# Planned Features

1. Lists: `key = [value1, value2, value3]`
2. Groups (collect sets of key-value pairs together to avoid name collisions).
3. Nested groups (subgroups within other groups).
4. Lists of groups.
5. Datetime values.

