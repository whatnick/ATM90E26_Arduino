# Support for STM32F1xx

There are STM32F103 based boards available at very affordable price-point, commonly referred to as the [Bluepill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill) as in the Matrix. The pin maps is available via [STM32Duino](https://github.com/stm32duino/Arduino_Core_STM32/blob/main/variants/STM32F1xx/F100C(8-B)T/variant_generic.h).

## SPI Mode
Using 1st bank of pins for SPI mode according to diagram [here](https://www.electronicshub.org/how-to-use-spi-in-stm32f103c8t6/).
SPI pins are in the table below
|Pin function| Pin Number|
|------------|-----------|
|MOSI | PA7 |
|MISO | PA6 |
|SCK | PA5 |
|CS | PA4 |

## UART Mode
**NOTE** - Pin functions are reversed on the ATM90E26 Side
## Software Serial - TBD
|Pin function (STM32)| Pin Number|
|------------|-----------|
|RX | 10 |
|TX | 11 |

### Hardware Serial - Tested Working
|Pin function (STM32) | Pin Number |
|------------|-----------|
|RX | PB11 |
|TX | PB10 |
