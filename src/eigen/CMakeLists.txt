FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)  
  FetchContent_Populate(eigen)
endif()

  add_subdirectory(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR} EXCLUDE_FROM_ALL)