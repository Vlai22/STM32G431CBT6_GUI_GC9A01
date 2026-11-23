#include "gui.h"

void Screen::lineBrezenheim(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* buffer, uint16_t* color){
    int16_t dx = abs(x1 - x2);// получаем число без знака разницы между точками для задания шагов
    int16_t dy = -abs(y1 - y2);
    const uint16_t thisColor = *color;
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
    // Считаем границы текущей секции один раз до цикла
    const uint16_t secH = this->h / this->section;
    const uint16_t yStart = this->nowSection * secH;
    const uint16_t yEnd = yStart + secH;
    while(1){
        if(x1 < this->w && y1 > yStart && y1 < yEnd){
            buffer[x1 + (y1 - yStart) * this->w] = thisColor;
        }
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
void Screen::circleBrezenheim(uint16_t x, uint16_t y, uint16_t r, uint8_t portion, uint8_t flags/*1: Fill*/, uint16_t* buffer, uint16_t* color, uint16_t* fill_color, uint16_t* border){
    int16_t err = 3 - 2 * r;
    int16_t thisx = 0;
    int16_t thisy = r;
    while(thisx <= thisy){
        if(err < 0){
            err += 4 * thisx + 6;
        }else{
            err += 4 * (thisx - thisy) + 10;
            thisy--;
        }
        thisx++;
    }
}