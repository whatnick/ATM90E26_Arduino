# Support for STM32F1xx

There are STM32F103 based boards available at very affordable price-point, commonly referred to as the [Bluepill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill) as in the Matrix. The pin maps is available via [STM32Duino](https://github.com/stm32duino/Arduino_Core_STM32/blob/main/variants/STM32F1xx/F100C(8-B)T/variant_generic.h).

## SPI Mode

SPI pins are in the table below
|Pin function| Pin Number|
|------------|-----------|
|MOSI | 11 |
|MISO | 12 |
|SCK | 13 |
|CS | 7 |

## UART Mode
|Pin function| Pin Number|
|------------|-----------|
|RX | 10 |
|TX | 11 |