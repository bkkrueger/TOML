module toml

    using PyCall
    @pyimport pytoml

    function load(filename::String)
        fin = pyeval(join(["open(\"", filename, "\", 'r')"]))
        toml_dict = pytoml.load(fin)
        fin[:close]()
        return(toml_dict)
    end

end
