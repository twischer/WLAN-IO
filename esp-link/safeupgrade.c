#include <esp8266.h>
#include "cgiflash.h"
#include "safeupgrade.h"

/* the timeout which will wait for the wifi flasher response
 * until the upgrade will be undone (in seconds)
 */
#define UPGRADE_FAILED_TIMEOUT          20

// TODO only works with 512KB flash
#define BOOTLOADER_CONFIG_ADDR  0x7F000

/* Should be a position which is not used by the 2nd stage bootloader.
 * But it has to be in the same flash block as the bootloader config
 */
#define BOOTLOADER_CONFIG_SUCCESS_BYTE_POS  0x2F
#define BOOTLOADER_CONFIG_SUCCESS_MAGIC     0xA5

void ICACHE_FLASH_ATTR cgiFlashSetUpgradeSuccessful(void) {
    uint8 bootloaderConfig[BOOTLOADER_CONFIG_SUCCESS_BYTE_POS + 1];
    spi_flash_read(BOOTLOADER_CONFIG_ADDR, (uint32*)bootloaderConfig, sizeof(bootloaderConfig));

    /* cancle, if already set */
    if (bootloaderConfig[BOOTLOADER_CONFIG_SUCCESS_BYTE_POS] == BOOTLOADER_CONFIG_SUCCESS_MAGIC) {
        return;
    }

    /* set the upgrade successful byte */
    bootloaderConfig[BOOTLOADER_CONFIG_SUCCESS_BYTE_POS] = BOOTLOADER_CONFIG_SUCCESS_MAGIC;

    spi_flash_erase_sector(BOOTLOADER_CONFIG_ADDR / SPI_FLASH_SEC_SIZE);
    spi_flash_write(BOOTLOADER_CONFIG_ADDR, (uint32*)bootloaderConfig, sizeof(bootloaderConfig));
}

static bool ICACHE_FLASH_ATTR cgiFlashIsUpgradeSuccessful(void) {
    uint8 bootloaderConfig[BOOTLOADER_CONFIG_SUCCESS_BYTE_POS + 1];
    spi_flash_read(BOOTLOADER_CONFIG_ADDR, (uint32*)bootloaderConfig, sizeof(bootloaderConfig));

    const bool successfull = (bootloaderConfig[BOOTLOADER_CONFIG_SUCCESS_BYTE_POS] == BOOTLOADER_CONFIG_SUCCESS_MAGIC);
    os_printf("Upgrade successful: %u\n", successfull);
    return successfull;
}

static void ICACHE_FLASH_ATTR undoUpgradeIfPossible(void* const timer) {
  /* remove timer, if called from timer */
  if (timer != NULL) {
    os_timer_disarm(timer);
  }

  /* Do not undo the upgrade, if it boots successfully ones */
  if (cgiFlashIsUpgradeSuccessful()) {
      return;
  }

  /* Do not undo the upgrade, if the old flash is no longer valid.
   * Because it was possibly already overwritten
   */
  if (checkUpgradedFirmware() != NULL) {
      return;
  }

  system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
  system_upgrade_reboot();
}

/**
  -1 downgrade done. Do not execute any user code after this call
   0 Possibly downgrade needed in the future
   1 Upgrading was successfully. No downgrade is needed
  */
int ICACHE_FLASH_ATTR cgiFlashCheckUpgradeHealthy() {
    /* Do not undo the upgrade, if it boots successfully ones */
    if (cgiFlashIsUpgradeSuccessful()) {
        return 1;
    }

    /* undo upgrade, if the first boot failes
     * with an watchdog reset, soft watchdog reset or an exception
     */
    struct rst_info *rst_info = system_get_rst_info();
    if (rst_info->reason >= 1 && rst_info->reason <= 3) {
        undoUpgradeIfPossible(NULL);
        return -1;
    }

    /* if the system do not boot vital in 20sec,
     * try to undo the upgrade
     */
    static ETSTimer upgradeFailedTimeout;
    os_timer_setfn(&upgradeFailedTimeout, undoUpgradeIfPossible, &upgradeFailedTimeout);
    os_timer_arm(&upgradeFailedTimeout, UPGRADE_FAILED_TIMEOUT * 1000, false);
    return 0;
}
