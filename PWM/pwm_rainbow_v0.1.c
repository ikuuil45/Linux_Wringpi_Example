#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>

const int Red_Pin=1;
const int Green_Pin=4;
const int Blue_Pin=5;
const int Key_Pin=7;

int i;
int Key_value;

int colors[][3] =
 {
    {255, 0, 0},     // 红色
    {230, 20, 0},    // 橙色
    {255, 30, 0},    // 黄色
    {0, 255, 0},     // 绿色
    {0, 250, 154},   // 青色
    {0, 0, 255},     // 蓝色
    {139, 0, 139}    // 紫色
};
int numColors = sizeof(colors) / sizeof(colors[0]);

char* colorNames[] =
 {
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple"
};

void GPIO_Init()
{
    pinMode(Key_Pin,INPUT);
    softPwmCreate(Red_Pin, 0, 255);
    softPwmCreate(Green_Pin, 0, 255);
    softPwmCreate(Blue_Pin, 0, 255); 
}


int main()
{

     if(wiringPiSetup() == -1)//树莓派引脚初始化，返回-1则初始化失败
     {
        printf("Error!\n");
        return 1;
     }
    GPIO_Init();/
    while(1)
    
    {
        //实现彩色渐变循环亮功能
        for (i=0; i < numColors; i++)
         {
            softPwmWrite(Red_Pin, colors[i][0]);
            softPwmWrite(Green_Pin, colors[i][1]);
            softPwmWrite(Blue_Pin, colors[i][2]);
           // printf("%s\n", colorNames[i]);
            delay(1000);

         }
        Key_value = digitalRead(Key_Pin);
        printf("Color_Cycle_success\n");
        printf("%d\n",Key_value);

        i=0;

    }

    return 0;

}

//
