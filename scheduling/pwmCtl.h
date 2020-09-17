/*
 * The MIT License (MIT)

Copyright (c) 2015 Jetsonhacks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// servoExample.cpp
#ifndef PWMEXAMPLE_H
#define PWMEXAMPLE_H
#include <JHPWMPCA9685.h>

typedef enum direction {
    left = 1,
    right = 2,
    center = 0,
    none = -1
}Direction;

void setmap_all();
void unmap_all();
void set_back_GPIO_valt();
void set_forward_GPIO_valt();
void set_lift_GPIO_valt();
void set_right_GPIO_valt();
char getkey();
void set_GPIO(const char* BCM_num);
void set_GPIO_value_high(const char* BCM_num);
void set_GPIO_value_low(const char* BCM_num);
void unmap_GPIO(const char* BCM_num);
bool is_closePerson();
float max_check(float value);
float min_check(float value);
float envirCam_checkPerson(int* change_1, int* change_2, float p_decelerate);

void* pwmctl_forward(void* );
int pwmctl_byChar();
Direction trackObject_direction();
void* presonFollow(void* );
void* reverseRoute_stop(void* );

bool check_PCA9685Error(int errorCode, PCA9685 *pca9685);

#endif

