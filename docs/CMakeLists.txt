find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_PROJECT_NAME "Patater GUI Kit")
    set(DOXYGEN_PROJECT_BRIEF "A toolkit for making GUIs for SDL and DOS")
    set(DOXYGEN_FULL_PATH_NAMES NO)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    set(DOXYGEN_SOURCE_BROWSER YES)

    get_target_property(GUIKIT_HEADERS guikit HEADER_SET_HEADERS)

    doxygen_add_docs(docs
        "${GUIKIT_HEADERS}"
        ALL
    )
endif()
