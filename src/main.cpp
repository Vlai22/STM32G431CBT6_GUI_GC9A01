#include "stm32g431xx.h"
#include "stm32g4xx_LL_rcc.h"
#include "stm32g4xx_LL_bus.h"
#include "stm32g4xx_LL_spi.h"
#include "stm32g4xx_LL_gpio.h"
#include "stm32g4xx_LL_pwr.h"
#include "stm32g4xx_LL_system.h"
#include "stm32g4xx_LL_tim.h"
#include "gui.h"

void SPIInit(){
    //включение тактирования необходимых агрегатов
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    while(LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI2) != 1);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    while(LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOB) != 1);
    //включение конкретных пинов для spi
    //----------------CHIP Select
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_12, LL_GPIO_PULL_DOWN);
    //------------------SCL
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_13, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_13, LL_GPIO_PULL_NO);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_13, LL_GPIO_AF_5);
    //-----------------------MISO
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_14, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_14, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_14, LL_GPIO_PULL_UP);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_14, LL_GPIO_AF_5);
    //-----------------------MOSI
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_15, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_15, LL_GPIO_AF_5);
    // выключаем spi для настройки 
    LL_SPI_Disable(SPI2);
    LL_SPI_SetMode(SPI2, LL_SPI_MODE_MASTER);
    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_1EDGE);
    LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_LOW);
    LL_SPI_SetBaudRatePrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV16);
    LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_TX);
    LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER);
    LL_SPI_SetNSSMode(SPI2, LL_SPI_NSS_SOFT);
    LL_SPI_Enable(SPI2);
}
void SPIWrite8bit(uint8_t data) {
    while (!LL_SPI_IsActiveFlag_TXE(SPI2));
    LL_SPI_TransmitData8(SPI2, data);
    while (LL_SPI_IsActiveFlag_BSY(SPI2));
}
void GC9A01_WriteCmd(uint8_t cmd) {
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10); // DC = 0 (Команда)
    SPI2_WriteByte(cmd);
}
void GC9A01_WriteData(uint8_t data) {
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_10);   // DC = 1 (Данные)
    SPI2_WriteByte(data);
}
void GC9A01_Init(void) {
    // 1. Аппаратный сброс (RST = PB11)
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
    Delay_ms(10);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
    Delay_ms(20);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
    Delay_ms(120);

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12); // CS = 0 (выбор дисплея)

    // 2. Обязательная сервисная инициализация контроллера
    GC9A01_WriteCmd(0xEF);
    GC9A01_WriteCmd(0xEB); GC9A01_WriteData(0x14);
    GC9A01_WriteCmd(0xFE);
    GC9A01_WriteCmd(0xEF);
    GC9A01_WriteCmd(0xEB); GC9A01_WriteData(0x14);

    GC9A01_WriteCmd(0x84); GC9A01_WriteData(0x40);
    GC9A01_WriteCmd(0x85); GC9A01_WriteData(0xFF);
    GC9A01_WriteCmd(0x86); GC9A01_WriteData(0xFF);
    GC9A01_WriteCmd(0x87); GC9A01_WriteData(0xFF);
    GC9A01_WriteCmd(0x88); GC9A01_WriteData(0x0A);
    GC9A01_WriteCmd(0x89); GC9A01_WriteData(0x21);
    GC9A01_WriteCmd(0x8A); GC9A01_WriteData(0x00);
    GC9A01_WriteCmd(0x8B); GC9A01_WriteData(0x80);
    GC9A01_WriteCmd(0x8C); GC9A01_WriteData(0x01);
    GC9A01_WriteCmd(0x8D); GC9A01_WriteData(0x01);
    GC9A01_WriteCmd(0x8E); GC9A01_WriteData(0xFF);
    GC9A01_WriteCmd(0x8F); GC9A01_WriteData(0xFF);

    GC9A01_WriteCmd(0xB6); GC9A01_WriteData(0x00); GC9A01_WriteData(0x20);
    GC9A01_WriteCmd(0x36); GC9A01_WriteData(0x48); // Ориентация и BGR
    GC9A01_WriteCmd(0x3A); GC9A01_WriteData(0x05); // 16-bit RGB565

    GC9A01_WriteCmd(0x90); GC9A01_WriteData(0x08); GC9A01_WriteData(0x08); GC9A01_WriteData(0x08); GC9A01_WriteData(0x08);
    GC9A01_WriteCmd(0xBD); GC9A01_WriteData(0x06);
    GC9A01_WriteCmd(0xBC); GC9A01_WriteData(0x00);
    GC9A01_WriteCmd(0xFF); GC9A01_WriteData(0x60); GC9A01_WriteData(0x01); GC9A01_WriteData(0x04);
    GC9A01_WriteCmd(0xC3); GC9A01_WriteData(0x13);
    GC9A01_WriteCmd(0xC4); GC9A01_WriteData(0x13);
    GC9A01_WriteCmd(0xC9); GC9A01_WriteData(0x22);
    GC9A01_WriteCmd(0xBE); GC9A01_WriteData(0x11);
    GC9A01_WriteCmd(0xE1); GC9A01_WriteData(0x10); GC9A01_WriteData(0x0E);
    GC9A01_WriteCmd(0xDF); GC9A01_WriteData(0x21); GC9A01_WriteData(0x0C); GC9A01_WriteData(0x02);

    // Гамма-коррекция
    GC9A01_WriteCmd(0xF0); GC9A01_WriteData(0x45); GC9A01_WriteData(0x09); GC9A01_WriteData(0x08); GC9A01_WriteData(0x08); GC9A01_WriteData(0x26); GC9A01_WriteData(0x2A);
    GC9A01_WriteCmd(0xF1); GC9A01_WriteData(0x43); GC9A01_WriteData(0x70); GC9A01_WriteData(0x72); GC9A01_WriteData(0x36); GC9A01_WriteData(0x37); GC9A01_WriteData(0x6F);
    GC9A01_WriteCmd(0xF2); GC9A01_WriteData(0x45); GC9A01_WriteData(0x09); GC9A01_WriteData(0x08); GC9A01_WriteData(0x08); GC9A01_WriteData(0x26); GC9A01_WriteData(0x2A);
    GC9A01_WriteCmd(0xF3); GC9A01_WriteData(0x43); GC9A01_WriteData(0x70); GC9A01_WriteData(0x72); GC9A01_WriteData(0x36); GC9A01_WriteData(0x37); GC9A01_WriteData(0x6F);

    GC9A01_WriteCmd(0xED); GC9A01_WriteData(0x1B); GC9A01_WriteData(0x0B);
    GC9A01_WriteCmd(0xAE); GC9A01_WriteData(0x77);
    GC9A01_WriteCmd(0xCD); GC9A01_WriteData(0x63);
    GC9A01_WriteCmd(0x70); GC9A01_WriteData(0x07); GC9A01_WriteData(0x07); GC9A01_WriteData(0x04); GC9A01_WriteData(0x0E); GC9A01_WriteData(0x0F); GC9A01_WriteData(0x09); GC9A01_WriteData(0x07); GC9A01_WriteData(0x08); GC9A01_WriteData(0x03);
    GC9A01_WriteCmd(0xE8); GC9A01_WriteData(0x34);

    GC9A01_WriteCmd(0x62); GC9A01_WriteData(0x18); GC9A01_WriteData(0x0D); GC9A01_WriteData(0x71); GC9A01_WriteData(0xED); GC9A01_WriteData(0x70); GC9A01_WriteData(0x70); GC9A01_WriteData(0x18); GC9A01_WriteData(0x0F); GC9A01_WriteData(0x71); GC9A01_WriteData(0xEF); GC9A01_WriteData(0x70); GC9A01_WriteData(0x70);
    GC9A01_WriteCmd(0x63); GC9A01_WriteData(0x18); GC9A01_WriteData(0x11); GC9A01_WriteData(0x71); GC9A01_WriteData(0xF1); GC9A01_WriteData(0x70); GC9A01_WriteData(0x70); GC9A01_WriteData(0x18); GC9A01_WriteData(0x13); GC9A01_WriteData(0x71); GC9A01_WriteData(0xF3); GC9A01_WriteData(0x70); GC9A01_WriteData(0x70);
    GC9A01_WriteCmd(0x64); GC9A01_WriteData(0x28); GC9A01_WriteData(0x29); GC9A01_WriteData(0xF1); GC9A01_WriteData(0x01); GC9A01_WriteData(0xF1); GC9A01_WriteData(0x00); GC9A01_WriteData(0x07);
    GC9A01_WriteCmd(0x66); GC9A01_WriteData(0x3C); GC9A01_WriteData(0x00); GC9A01_WriteData(0xCD); GC9A01_WriteData(0x67); GC9A01_WriteData(0x45); GC9A01_WriteData(0x45); GC9A01_WriteData(0x10); GC9A01_WriteData(0x00); GC9A01_WriteData(0x00); GC9A01_WriteData(0x00);
    GC9A01_WriteCmd(0x67); GC9A01_WriteData(0x00); GC9A01_WriteData(0x3C); GC9A01_WriteData(0x00); GC9A01_WriteData(0x00); GC9A01_WriteData(0x00); GC9A01_WriteData(0x01); GC9A01_WriteData(0x54); GC9A01_WriteData(0x10); GC9A01_WriteData(0x32); GC9A01_WriteData(0x98);
    GC9A01_WriteCmd(0x74); GC9A01_WriteData(0x10); GC9A01_WriteData(0x85); GC9A01_WriteData(0x80); GC9A01_WriteData(0x00); GC9A01_WriteData(0x00); GC9A01_WriteData(0x4E); GC9A01_WriteData(0x00);
    GC9A01_WriteCmd(0x98); GC9A01_WriteData(0x3E); GC9A01_WriteData(0x07);

    GC9A01_WriteCmd(0x35); // Tearing Effect Line ON
    GC9A01_WriteCmd(0x21); // Инверсия цветов (для IPS)

    // 3. Выход из сна и включение экрана
    GC9A01_WriteCmd(0x11); // SLPOUT (именно 0x11!)
    Delay_ms(120);

    GC9A01_WriteCmd(0x29); // DISPON
    Delay_ms(20);
}
void RCC_170_Init(){
    //включаем внутрений источник частоты для работы впринципе контроллера
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1);
    //включаем тактирование на pwr модуль отвечающй за управление питанием и включаем тактирование памяти 
    // по скольку видимо на всех stm32 там есть режимы работы памяти
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    while(LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) != 1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_FLASH);
    while(LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_FLASH) != 1);
    LL_FLASH_EnablePrefetch();
    LL_FLASH_EnableInstCache();
    LL_FLASH_EnableDataCache();
    //по мануалу необходимо сначала выставить AHB делтель на 2 пред выставлением частоты на 170 мгц
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    // включаем буст по питанию 
    LL_PWR_EnableRange1BoostMode();
    while(LL_PWR_IsActiveFlag_VOS() == 1); 
    //включаем другой режим работы памяти
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4);
    //включаем внешний источник тактирования и убеждаемся в его работе 
    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1);
    // выключаем PLL настраиваем его
    LL_RCC_PLL_Disable();
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE,LL_RCC_PLLM_DIV_2, 85, LL_RCC_PLLR_DIV_2);
    // выходная частота обязаня делиться минимум на 2 на SYSTEMCLK п этому мы входное HSE 
    // делим на 2 что бы получить 4 и умножаем на 85 получаем 340 при делении на 2 получаем максимальную частоту ядра 
    // 170 МГц
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != 1);
    // выбор системного источника PLL
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    // выставляем делители шин тактирования но предварительно дожидаемся стабилизации 
    for(volatile int i = 0; i < 1000; i++) {
        __NOP();
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
}

int main(){
    RCC_170_Init();
    //включаем тактирования линий 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    while(LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOA) != 1);
    //настраиваем все пины для управления экраном 
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_8, LL_GPIO_AF_6);
    //-----------------------
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_10, LL_GPIO_PULL_DOWN);
    //-----------------------
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_11, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_DOWN);
    SPIInit();
    GC9A01_Init();
    while(true){
        uint16_t color = 0xF800;
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
        //------------------------
        GC9A01_WriteCmd(SPI2, 0x2A);// команда на задание размера по 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0xEF);// данные о конце кординат 
        GC9A01_WriteCmd(SPI2, 0x2B);// команда на задание размера по Y 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0x00);// данные о начале кординат 
        GC9A01_WriteData(SPI2, 0xEF);// данные о конце кординат 
        GC9A01_WriteCmd(SPI2, 0x2C);//команда на передачу данных о цвете 
        for(volatile uint16_t i = 0; i< 57600; i++){
            GC9A01_WriteData(SPI2, color >> 8);
            GC9A01_WriteData(SPI2, color & 0xFF);
        }
    }   
}