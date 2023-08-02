/*轮询版本
+-------------------+
|   GPIO_Init()     |
+-------------------+
         |
         v
+-------------------+
| wiringPiSetup()   |
+-------------------+
         |
         v
+-------------------+
|       main()      |
+-------------------+
         |
         v
+-------------------+
|  digitalRead()    |
+-------------------+
         |
         v
+-------------------+*
| redLightBlink()   |
+-------------------+
         |
         v
+-------------------+
| colorCycle()      |
+-------------------+
*/

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>

const int Red_Pin=1;
const int Green_Pin=4;
const int Blue_Pin=5;
const int Key_Pin=2;

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
    GPIO_Init();

while (1)
{

    Key_value = digitalRead(Key_Pin); //引脚查询
    if (Key_value == 0)
    {
        // 红灯爆闪
        for (i = 0; i < 10; i++)
        {
            softPwmWrite(Red_Pin, 0);
            softPwmWrite(Green_Pin, 0);
            softPwmWrite(Blue_Pin, 0);
            softPwmWrite(Red_Pin, 255);
            printf("The level is %d\n",Key_value);
            delay(100);
            softPwmWrite(Red_Pin, 0);
            delay(100);
        }
    }
    else
    {
        // 七色灯转换
        for (i = 0; i < numColors; i++)
        {
            softPwmWrite(Red_Pin, colors[i][0]);
            softPwmWrite(Green_Pin, colors[i][1]);
            softPwmWrite(Blue_Pin, colors[i][2]);
            delay(1000);
            printf("%s\t", colorNames[i]);
        }
         printf("Color_Cycle_success\n");
         printf("The level is %d\n",Key_value);
    }
}
    return 0;
}

