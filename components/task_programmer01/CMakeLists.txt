set(app_sources
	"task_programmer01.c"
	)

set(include_dirs
	"./include"
	)

idf_component_register(
	SRCS ${app_sources}
	INCLUDE_DIRS ${include_dirs}
#	REQUIRES nvs_flash spi_flash task_programmer01 wifi01
	)
