macro(build_tests TESTS_DIR)
	option(BUILD_TESTS "Build tests." $ENV{BUILD_TESTS})

	if(BUILD_TESTS)
		add_definitions(-DBUILD_TESTS)
		add_subdirectory(${TESTS_DIR})
	else()
		status_message("For building tests use -DBUILD_TESTS=Yes option.")
	endif()
endmacro()