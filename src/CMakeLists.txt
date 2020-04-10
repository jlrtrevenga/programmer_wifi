set(app_sources
	"main_programmer.c"
	)

set(include_dirs
	"."
	)

idf_component_register(
	SRCS ${app_sources}
	INCLUDE_DIRS ${include_dirs}
	)



