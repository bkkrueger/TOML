module toml

    using PyCall
    @pyimport pytoml

    function load(filename::String)
        fin = pyeval(join(["open(\"", filename, "\", 'r')"]))
        toml_dict = pytoml.load(fin)
        fin[:close]()
        return(toml_dict)
    end

    function dump(filename::String, toml_dict::Dict{Any,Any})
        fout = pyeval(join(["open(\"", filename, "\", 'w')"]))
        pytoml.dump(fout, toml_dict)
        fout[:close]()
    end

end
