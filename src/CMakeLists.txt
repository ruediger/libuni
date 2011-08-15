
set(generator_sources
  generate_two_stage_table.c++)

add_definitions("-DUCD_PATH=\"${UCD_PATH}\"")
add_definitions("-DOUTDIR=\"${libuni_SOURCE_DIR}/src/generated/\"")
if(UCD_VERSION)
  add_definitions("-DUCD_VERSION=\"${UCD_VERSION}\"")
endif()
add_executable(generator ${generator_sources})
get_target_property(generator_EXE generator LOCATION)
add_custom_command(
  OUTPUT ${libuni_SOURCE_DIR}/src/generated/normalization_database.hpp ${libuni_SOURCE_DIR}/src/generated/case_database.hpp ${libuni_SOURCE_DIR}/src/generated/segmentation_database.hpp
  COMMAND ${generator_EXE}
  DEPENDS generator
)

set(library_sources
  generated/normalization_database.hpp
  normalization.c++
  generated/case_database.hpp
  case.c++
  generated/segmentation_database.hpp
  segmentation.c++
  )

add_library(uni SHARED ${library_sources})

