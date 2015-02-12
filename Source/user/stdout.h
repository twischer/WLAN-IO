void stdoutInit();
void ICACHE_FLASH_ATTR uart_tx_one_char(uint8 uart, uint8 TxChar);
void ICACHE_FLASH_ATTR uart0_tx_buffer(uint8 *buf, uint16 len);