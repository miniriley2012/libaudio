get_directory_property(THIS_PROJECT_PARENT_DIR PARENT_DIRECTORY)
if (NOT THIS_DIRECTORY_PARENT_DIR)
    set(THIS_PROJECT ${PROJECT_NAME})
endif ()