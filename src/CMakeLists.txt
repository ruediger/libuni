
set(generator_sources
  generate_two_stage_table.c++)

add_executable(generator ${generator_sources})
get_target_property(generator_EXE generator LOCATION)

add_custom_command(
  OUTPUT ${libuni_SOURCE_DIR}/generated/normalization_database.hpp
  COMMNAD ${generator_EXE}
  WORKING_DIRECTORY ${libuni_SOURCE_DIR}
  DEPENDS generator
)

set(library_sources
  generated/normalization_database.hpp
  normalization.c++)

add_library(uni SHARED ${library_sources})

