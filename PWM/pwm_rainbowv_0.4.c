/*多线程版本
+-------------------+
|    GPIO_Init()    |
+-------------------+
         |
         v
+-------------------+
| wiringPiSetup()   |
+-------------------+
         |
         v
+-------------------+
|      main()       |
+-------------------+
         |
         |         +-------------------+
         +-------->|  pthread_create() |
         |         | redLightBlink()   |
         |         +-------------------+
         |                   |
         |                   v
         |         +-------------------+
         +-------->|  pthread_create() |
                   |   colorCycle()    |
                   +-------------------+
         |
         v
+-------------------+
|     while(1)      |
|   printf()        |
+-------------------+

首先进行GPIO初始化和wiringPi库的设置。然后进入main()函数，在main()函数中创建两个线程：redLightBlink()和colorCycle()。

这两个线程是并行执行的，并且它们之间没有明显的依赖关系。每个线程都会独立执行自己的任务，不会相互阻塞或等待。

在main()函数中，还有一个无限循环，用于保持主线程的执行。

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>
#include <unistd.h>


const int Red_Pin=1;
const int Green_Pin=4;
const int Blue_Pin=5;
const int Key_Pin=2;

int i;
int Key_value;
int prev_value;
int current_value;

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


/*这部分应该单独放置一个函数，后面用Makefile来将所有函数链接*/

void* handleKeyPress(void* arg);
void* redLightBlink(void* arg);
void* colorCycle(void* arg);

void GPIO_Init()
{
    pinMode(Key_Pin,INPUT);
    softPwmCreate(Red_Pin, 0, 255);
    softPwmCreate(Green_Pin, 0, 255);
    softPwmCreate(Blue_Pin, 0, 255); 
}


void* handleKeyPress(void* arg)
{
    while (1)
    {   
        current_value = digitalRead(Key_Pin);
        if ((prev_value == 0 && current_value == 1) || (prev_value == 1 && current_value == 0))
        {
            prev_value = current_value;
            printf("handleKeyPress running \n");
        }
        delay(1000); // 轮询间隔
    }

    return NULL;
}

void* redLightBlink(void* arg)
{
    while (1)
    {
        printf("redLightBlink is Running!\n");
        delay(1000);
        if (prev_value == 0)
        {
            softPwmWrite(Red_Pin, 0);
            softPwmWrite(Green_Pin, 0);
            softPwmWrite(Blue_Pin, 0);
            softPwmWrite(Red_Pin, 255);
            delay(100);
            printf("Blink Success\n");
            if (prev_value == 1)
            {
                printf("Blink Exit\n");
                break;
            }
            softPwmWrite(Red_Pin, 0);
            delay(100);
        }
    }

    return NULL;
}

void* colorCycle(void* arg)
{
    while (1)
    {
        printf("colorCycle_Thread is Running!\n");
        delay(1000);
        if (prev_value == 1)
        {
            for (i = 0; i < numColors; i++)
            {
                softPwmWrite(Red_Pin, colors[i][0]);
                softPwmWrite(Green_Pin, colors[i][1]);
                softPwmWrite(Blue_Pin, colors[i][2]);
                printf("Cycle Success\n");
                if (prev_value == 0)
                {
                    printf("Cycle Exit\n");
                    break;
                }
                delay(1000);
                printf("%s\n", colorNames[i]);
            }
        }
    }

    return NULL;
}

int main()
{
    if (wiringPiSetup() == -1)
    {
        printf("Error!\n");
        return 1;
    }

    pthread_t keyPressThread, blinkThread, cycleThread;

    GPIO_Init();

    // 创建线程
    pthread_create(&keyPressThread, NULL, handleKeyPress, NULL);
    pthread_create(&blinkThread, NULL, redLightBlink, NULL);
    pthread_create(&cycleThread, NULL, colorCycle, NULL);

    while (1)
    {
        printf("Main_Thread is Running!\n"); // 主线程持续执行
        delay(1000);
    }

    return 0;
}