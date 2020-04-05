set(app_sources 
	"wifi01.c"
	)

set(comp_includes
	"./include"
	"C:/Users/jlrtr/.platformio/packages/framework-espidf/components/nvs_flash/include"
	"C:/Users/jlrtr/.platformio/packages/framework-espidf/components/spi_flash/include"
	)

idf_component_register(
		SRCS ${app_sources}
		INCLUDE_DIRS ${comp_includes}
#		REQUIRES nvs_flash spi_flash
		)