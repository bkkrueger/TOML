#include <iostream>
#include <string>

#include "config.h"

int main(int argc, char *argv[]) {

    std::string filename = "eta000.config";

    Config::Table table, subtable;
    table.parse_file(filename);

    subtable = table.get_table("field");
    Config::Float field__turb_ener_frac =
        subtable.get_scalar("turb_ener_frac").as_float();
    Config::Float field__spectral_index = 
        subtable.get_scalar("spectral_index").as_float();
    Config::Float field__max_wave = 
        subtable.get_scalar("max_wave").as_float();
    Config::Float field__min_wave = 
        subtable.get_scalar("min_wave").as_float();
    Config::Float field__wave_resolution = 
        subtable.get_scalar("wave_resolution").as_float();
    Config::Float field__wave_speed = 
        subtable.get_scalar("wave_speed").as_float();

    subtable = table.get_table("experiment");
    Config::String experiment__name =
        subtable.get_scalar("name").as_string();
    Config::String experiment__notes =
        subtable.get_scalar("notes").as_string();
    Config::Float experiment__max_time =
        subtable.get_scalar("max_time").as_float();
    Config::Integer experiment__number_of_particles =
        subtable.get_scalar("number_of_particles").as_integer();
    Config::Integer experiment__particle_seed =
        subtable.get_scalar("particle_seed").as_integer();
    Config::Integer experiment__number_of_fields =
        subtable.get_scalar("number_of_fields").as_integer();
    Config::Integer experiment__field_seed =
        subtable.get_scalar("field_seed").as_integer();
    Config::String experiment__experiment_directory =
        subtable.get_scalar("experiment_directory").as_string();
    Config::String experiment__field_stub =
        subtable.get_scalar("field_stub").as_string();
    Config::String experiment__particle_stub =
        subtable.get_scalar("particle_stub").as_string();
    Config::Float experiment__step_small =
        subtable.get_scalar("step_small").as_float();
    Config::Integer experiment__max_steps =
        subtable.get_scalar("max_steps").as_integer();

    std::cout << "data extracted from file " << filename << ":" << std::endl;

    std::cout << "    field data:" << std::endl;
    std::cout << "        turb_ener_frac  = " << field__turb_ener_frac
        << std::endl;
    std::cout << "        spectral_index  = " << field__spectral_index
        << std::endl;
    std::cout << "        max_wave        = " << field__max_wave << std::endl;
    std::cout << "        min_wave        = " << field__min_wave << std::endl;
    std::cout << "        wave_resolution = " << field__wave_resolution
        << std::endl;
    std::cout << "        wave_speed      = " << field__wave_speed
        << std::endl;

    std::cout << "    experiment data:" << std::endl;
    std::cout << "        name                 = " << experiment__name
        << std::endl;
    std::cout << "        notes                = " << experiment__notes
        << std::endl;
    std::cout << "        max_time             = " << experiment__max_time
        << std::endl;
    std::cout << "        number_of_particles  = "
        << experiment__number_of_particles << std::endl;
    std::cout << "        particle_seed        = " << experiment__particle_seed
        << std::endl;
    std::cout << "        number_of_fields     = "
        << experiment__number_of_fields << std::endl;
    std::cout << "        field_seed           = " << experiment__field_seed
        << std::endl;
    std::cout << "        experiment_directory = "
        << experiment__experiment_directory << std::endl;
    std::cout << "        field_stub           = " << experiment__field_stub
        << std::endl;
    std::cout << "        particle_stub        = " << experiment__particle_stub
        << std::endl;
    std::cout << "        step_small           = " << experiment__step_small
        << std::endl;
    std::cout << "        max_steps            = " << experiment__max_steps
        << std::endl;

    return 0;
}
