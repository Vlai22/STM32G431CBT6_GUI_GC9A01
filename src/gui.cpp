#include "gui.h"

uint16_t Screen::alphaBlend(uint16_t bgcolor, uint16_t color, uint8_t alpha){
    if(alpha == 0){
        return  bgcolor;
    }else if(alpha == 255){
        return color; 
    }
    //битовая маска работает следующим образом какие то значения у нас находяться в 16 битном числе везде где нам нужны 
    //данные пишем единицы и таким образом оставляем при операции И те же данные которые были а те что нам нужны остаються
    uint16_t rBg = (bgcolor >> 11) & 0x1F;// сдвигаем биты цвета вправо и пременяем битовую маску что бы выделить 
    // только один цвет
    uint16_t gBg = (bgcolor >> 5) & 0x3F;// сдвигаем биты цвета вправо и пременяем битовую маску что бы выделить 
    // только один цвет отличаеться от верхнего и нижнего в том что тут 6 бит данных цвета
    uint16_t bBg = bgcolor & 0x1F;// пременяем битовую маску что бы выделить только один цвет
    uint16_t r = (color >> 11) & 0x1F;
    uint16_t g = (color >> 5) & 0x3F;
    uint16_t b = color & 0x1F;
    uint16_t inv_alpha = 255 - alpha;//инвертируем альфа канал чтобы получить сколько будет цвета бэграунда
    r = (r * alpha + rBg * inv_alpha) / 255;// складываем цвета и востанавливем их что бы они не сдвинулись
    g = (g * alpha + gBg * inv_alpha) / 255;
    b = (b * alpha + bBg * inv_alpha) / 255;
    return (r << 11) | (g << 5) | b;
}

void Screen::drawPixelinBuffer(uint16_t x, uint16_t y, uint16_t color, uint8_t alpha, uint16_t* buffer){
    // Считаем границы текущей секции один раз до цикла
    uint16_t secH = this->h / this->section;
    uint16_t yStart = this->nowSection * secH;
    uint16_t yEnd = yStart + secH;
    if(x < this->w && y >= yStart && y < yEnd){
        uint16_t bgcolor = buffer[x + (y - yStart) * this->w];
        buffer[x + (y - yStart) * this->w] = alphaBlend(bgcolor, color, alpha);
    }
}

void Screen::lineBrezenheim(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* buffer, uint16_t* color, uint8_t alpha){
    int16_t dx = abs(x1 - x2);// получаем число без знака разницы между точками для задания шагов
    int16_t dy = -abs(y1 - y2);
    int16_t err = dx + dy;// задаем начальную ошибку
    int8_t sy = 0;
    int8_t sx = 0;
    if(x1 > x2){// вычислаем шаг по x и y в плюс или миннус
        sx = -1;
    }else{
        sx = 1;
    }
    if(y1 > y2){
        sy = -1;
    }else{
        sy = 1;
    }
    while(1){
        Screen::drawPixelinBuffer(x1, y1, *color, alpha, buffer);
        if(x1 == x2 && y1 == y2){//если дошли до конечной точки значит все отрисовали
            break;
        }
        int16_t e2 = 2 * err;//удваиваем ошибку что бы избаивиться от деления и соответсвенно дробей
        // алгорим проверяет если коэфицент ошибки больше dx или dy поделенного на 2 тогда мы делаем доп шажок
        if (e2 >= dy) {
            err += dy;// обновлаем пложение по x и обновляем ошибку
            x1 += sx;
        }
        if (e2 <= dx) { 
            err += dx;// обновлаем пложение по x и обновляем ошибку
            y1 += sy;
        }
    }
}

void Screen::circleBrezenheim(uint16_t x, uint16_t y, uint16_t r, uint8_t portion, uint8_t flags/*1: Fill*/, uint16_t* buffer, uint16_t* color, uint16_t* fill_color, uint16_t* border, uint8_t* alpha){
    int16_t err = 3 - 2 * r;
    int16_t thisx = 0;
    int16_t thisy = r;
    while(thisx <= thisy){

        if(portion & (1 << 0)){
            Screen::drawPixelinBuffer(x1, y1, *color, alpha, buffer);
        }else if(portion & (1 << 1)){

        }else if(portion & (1 << 2)){

        }else if(portion & (1 << 3)){

        }else if(portion & (1 << 4)){

        }else if(portion & (1 << 5)){

        }else if(portion & (1 << 6)){

        }else if(portion & (1 << 7)){

        }
        if(err < 0){
            err += 4 * thisx + 6;
        }else{
            err += 4 * (thisx - thisy) + 10;
            thisy--;
        }
        thisx++;
    }
}