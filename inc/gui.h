#include "stm32g431xx.h"

class Screen{
    public:
    uint16_t w = 240;
    uint16_t h = 240;
    uint8_t section = 24;
    uint8_t nowSection = 0;
    uint8_t wigetRedrawFifo[16];
    uint16_t buffer1[this->w * (this->h / 24)];
    uint16_t buffer2[this->w * (this->h / 24)];
    void cordicGet();
    void drawPixelinBuffer(uint16_t x, uint16_t y, uint16_t color, uint8_t alpha, uint16_t* buffer);
    void alphaBlend(uint16_t bgcolor, uint16_t color, uint8_t alpha);
    void lineBrezenheim(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* buffer, uint16_t* color, uint8_t alpha);
    void circleBrezenheim(uint16_t x, uint16_t y, uint16_t r, uint8_t portion, uint8_t flags/*1: Fill*/, uint16_t* buffer, uint16_t* color, uint16_t* fill_color, uint16_t* border, uint8_t* alpha); 
    void lineVu(uint16_t* x1, uint16_t* y1, uint16_t* x2, uint16_t* y2 uint16_t* buffer, uint16_t color);
    void circleVu(uint16_t* x, uint16_t* y, uint16_t* r, uint8_t portion, uint8_t flags/*1: Fill*/, uint16_t* buffer, uint16_t* color, uint16_t* fill_color, uint16_t* border);
    //void iconUpload(uint16_t* x, uint16_t* y, uint16_t* h, uint16_t* w, uint16_t* id, uint16_t* buffer);
    void draw();
}

struct _flagsWgt{
    uint8_t readraw : 1; // перерисовка
    uint8_t isPressed : 1; // нажатие 
    uint8_t isActive : 1; // активна для нажатий и тд 
    uint8_t isBacground : 1; // есть фон
    uint8_t isVisible : 1; // видима
    uint8_t focusAble : 1; // есть возможность навестись
    uint8_t focus : 1; // нведено
    uint8_t border : 1; // граница
};

class Wiget{
    public:
    uint8_t id;
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t h = 0;
    uint16_t w = 0;
    _flagsWgt flagsWgt = 0x00;
    uint8_t radius = 0;
    uint8_t border_w = 0;
    uint16_t color = 0x0000;
    uint16_t bgcolor = 0x0000;
    uint16_t border_color = 0x0000;
    uint8_t alpha = 100;
};
class Button : Wiget{

};


