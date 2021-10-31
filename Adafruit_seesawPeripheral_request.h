#define SEESAW_HW_ID_CODE_TINY8x7 0x87

/***************************** data read */
void requestEvent(void) {  
  //SEESAW_DEBUGLN(F("Requesting data"));

  uint8_t base_cmd = i2c_buffer[0];
  uint8_t module_cmd = i2c_buffer[1];

  if (base_cmd == SEESAW_STATUS_BASE) {
    if (module_cmd == SEESAW_STATUS_HW_ID) {
       Wire.write(SEESAW_HW_ID_CODE_TINY8x7);
    }
    if (module_cmd == SEESAW_STATUS_VERSION) {
      Adafruit_seesawPeripheral_setDatecode();
      Adafruit_seesawPeripheral_write32(CONFIG_VERSION | DATE_CODE);
    }
  }
  else if (base_cmd == SEESAW_GPIO_BASE) {
    if (module_cmd == SEESAW_GPIO_BULK) {
       Adafruit_seesawPeripheral_write32(Adafruit_seesawPeripheral_readBulk(VALID_GPIO));
    }
#if CONFIG_INTERRUPT
    else if (module_cmd == SEESAW_GPIO_INTFLAG) {
       Adafruit_seesawPeripheral_write32(g_irqFlags);
       g_irqFlags = 0; // reading the flags clears them
    }
#endif
  }

#if CONFIG_ADC
  else if (base_cmd == SEESAW_ADC_BASE) {
    uint32_t temp = 0xFFFF;
    if (module_cmd >= SEESAW_ADC_CHANNEL_OFFSET) {
      uint8_t adcpin = module_cmd - SEESAW_ADC_CHANNEL_OFFSET;
      if (! ((VALID_ADC) & (1UL << adcpin))) {
        g_adcStatus = 0x1; // error, invalid pin!
      } else {
        // its valid!
        SEESAW_DEBUG(F("ADC read "));
        SEESAW_DEBUG(adcpin);
        SEESAW_DEBUG(F(": "));
        temp = analogRead(adcpin);
        SEESAW_DEBUGLN(temp);
        g_adcStatus = 0x0;
      }
      Wire.write(temp >> 8);
      Wire.write(temp);
    }
    else if (module_cmd == SEESAW_ADC_STATUS) {
      Wire.write(g_adcStatus);
    }
  }
#endif

#if CONFIG_EEPROM
  else if (base_cmd == SEESAW_EEPROM_BASE) {
    Wire.write(EEPROM.read(module_cmd % EEPROM.length()));
  }
#endif
  else {
    SEESAW_DEBUG(F("Unhandled cmd 0x"));
    SEESAW_DEBUGLN(base_cmd, HEX);
  }
}
