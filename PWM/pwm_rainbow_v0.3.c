/*稳定的中断版本

+-------------------+       +-------------------+
|    GPIO_Init()    |       |   keyInterrupt()  |
+-------------------+       +-------------------+
         |                          |
         v                          v
+-------------------+       +-------------------+
| redLightBlink()   |       |                   |
+-------------------+       |                   |
         |                  |                   |
         v                  |                   |
+-------------------+       |                   |
|   colorCycle()    |       |                   |
+-------------------+       |                   |
         |                  |                   |
         v                  |                   |
+-------------------+       |                   |
|     main()        |       |                   |
+-------------------+       +-------------------+
         ^                          |
         |                          |
         +--------------------------+

在这个流程图中，keyInterrupt()函数被放在了主循环的右边，以突出显示它的中断特性。当按键发生状态变化时，中断会触发执行keyInterrupt()函数。

在keyInterrupt()函数执行完毕后，控制权会返回到主程序的下一步，即继续执行redLightBlink()和colorCycle()函数。

其他部分的流程和之前的描述一致，redLightBlink()函数在按键按下时闪烁红色LED灯，colorCycle()函数在按键抬起时循环显示彩虹颜色，GPIO_Init()函数用于初始化GPIO引脚和软件PWM。
*/


#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>
#include <unistd.h>
/*
*/

const int Red_Pin=1;
const int Green_Pin=4;
const int Blue_Pin=5;
const int Key_Pin=2;

int i;
int Key_value;
int prev_value;
int current_value;

/**/
void keyInterrupt(void);
void redLightBlink();
void colorCycle();


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

/*
GPIO并不是所有的都支持外部中断输入，前面轮询中的GPIO7是不可以的，会出现
gpio: Unable to open GPIO direction interface for pin 4: No such file or directory 
wiringPiISR: unable to open /sys/class/gpio/gpio4/value: No such file or directory
这是因为
*/
void GPIO_Init()
{
    pinMode(Key_Pin,INPUT);
    softPwmCreate(Red_Pin, 0, 255);
    softPwmCreate(Green_Pin, 0, 255);
    softPwmCreate(Blue_Pin, 0, 255); 
}

/*
这边不去针对于某一个边沿采取的是边沿只要有变化就把键值更新到prev_value
这样可以避免main循环中的更多混乱

*/

/*
Interrupt中只处理标志位置，且尽可能不出现dealy，应用函数全部都放在Callback中运行
目前的Callback有redLightBlink()，colorCycle()
*/
void keyInterrupt()
{
    prev_value = 0; // 保存上一次的采样值

    current_value = digitalRead(Key_Pin); // 当前的采样值

    // 判断是否发生01转换
    if ((prev_value == 0 && current_value == 1) || (prev_value == 1 && current_value == 0))
    {
        // 在发生01转换时执行的代码

        // 更新prev_value为当前的采样值
        prev_value = current_value;
    }
}



/*
Red 闪烁函数
这边的初始化全部置零的逻辑看着有点繁杂，需要进一步优化
另外
*/
void redLightBlink()
{
    if (prev_value == 1)
    {
        
        softPwmWrite(Red_Pin, 0);
        softPwmWrite(Green_Pin, 0);
        softPwmWrite(Blue_Pin, 0);
        softPwmWrite(Red_Pin, 255);
        delay(100);
        printf("Blink Success\n");
        // 判断是否发生01转换
        if (prev_value == 0)
        {
            // 按键被按下
            printf("Blink Exit\n");
            // 跳出函数
            //prev_value=1; 
           return;
        }
        softPwmWrite(Red_Pin, 0);
        delay(100);
    }
}

/*
Rainbow 函数
*/
void colorCycle()
{
    if (prev_value == 0)
    {
        for (i = 0; i < numColors; i++)
        {
            softPwmWrite(Red_Pin, colors[i][0]);
            softPwmWrite(Green_Pin, colors[i][1]);
            softPwmWrite(Blue_Pin, colors[i][2]);
            printf("Cycle Success\n");
            // 判断是否发生01转换，点控按键摆脱边沿的限制

            if (prev_value == 1)
            {
                // 按键抬起
                printf("Cycle Exit\n");
                // 清除中断标志位
                //Key_value  = 0;
                //prev_value=0;
                // 跳出函数
                return;
            }
            delay(1000);
            printf("%s\n", colorNames[i]);
        }

    }
}

/*
目前控制实现OK
应用功能：电平切换RGB的不同函数功能
int main()
后续可以改进为可以传参数的C&C++标准形式
int main（int arc，char* arv[]）
*/
int main()
{

     if(wiringPiSetup() == -1)//树莓派引脚初始化，返回-1则初始化失败
     {
        printf("Error!\n");
        return 1;
     }
    
    GPIO_Init();
    wiringPiISR(Key_Pin,INT_EDGE_FALLING,&keyInterrupt);

    while (1)
    {
        colorCycle();
        redLightBlink();
        printf("main running \n");
    }
    return 0;
}