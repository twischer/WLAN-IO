#include "osapi.h"
#include "user_interface.h"
#include "paramconf.h"

#define ESP_PARAM_START_SEC		0x3D

#define MAGIC_NUMBER			0xA3


bool ICACHE_FLASH_ATTR
paramconf_load(parameter_t* const param)
{
	spi_flash_read(ESP_PARAM_START_SEC * SPI_FLASH_SEC_SIZE, (uint32 *)param, sizeof(parameter_t));

	return (param->magic == MAGIC_NUMBER);
}

void ICACHE_FLASH_ATTR
paramconf_save(parameter_t* const param)
{
	param->magic = MAGIC_NUMBER;

	spi_flash_erase_sector(ESP_PARAM_START_SEC);
	spi_flash_write(ESP_PARAM_START_SEC * SPI_FLASH_SEC_SIZE, (uint32 *)param, sizeof(parameter_t));
}
