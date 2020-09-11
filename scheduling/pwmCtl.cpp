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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <JHPWMPCA9685.h>

//GPIO
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <pwmCtl.h>

#include <tuple>
extern int g_detecting_person;
extern pthread_mutex_t mute_pi_person;

extern std::tuple<float, float> g_objection_center;
extern pthread_mutex_t mute_objection_center;
extern float g_object_width;
extern float g_object_high;


int duty_cycle = 4095 ;
const char N1[] = "200";
const char N2[] = "38";
const char N3[] = "76";
const char N4[] = "12";
const std::tuple<float, float> zoreTuple = {0.0, 0.0};


void* presonFollow(void* ){
    PCA9685 *pca9685 = new PCA9685() ;
    int err = pca9685->openPCA9685();
    setmap_all();
    Direction enum_dir;
 
    if(check_PCA9685Error(err, pca9685)){
        printf("PCA9685 Device Address: 0x%02X\n",pca9685->kI2CAddress) ;
        pca9685->setAllPWM(0,0) ;
        pca9685->reset() ;
        pca9685->setPWMFrequency(60) ;

        float speed_ratio = 0;
        float decelerate = 0;
        float cur_speed = 0;

        int change_1 = 1;
        int change_2 = 1;
        while(true)
        {   
            enum_dir = trackObject_direction();
            
            if(enum_dir == center){
                 set_forward_GPIO_valt();
                 speed_ratio = 1;
                // printf("forwarding ! 1.0\n");
            }
            else if(enum_dir == right){
                 set_right_GPIO_valt();
                 speed_ratio = 0.9;
                // printf("right ! 0.9\n");
            }else if(enum_dir == left){
                 set_lift_GPIO_valt();
                 speed_ratio = 0.9;
               //  printf("left ! 0.9\n");
            }else if(enum_dir == none){
                speed_ratio = 0;
            }
            if(is_closePerson()){
                //speed_ratio = 0;
            }

            char input_c = getkey();
            if(input_c == 'w'){
                speed_ratio += 0.1;
                printf("speed_ratio: %02f\n",speed_ratio) ;
            }
            if(input_c == 's'){
                speed_ratio -= 0.1;
                printf("speed_ratio: %02f\n",speed_ratio) ;
            }
            if(input_c == 'q'){
                pca9685->setPWM(0, 0, (float)0);
                pca9685->setPWM(1, 0, (float)0);
                break;
            }
            decelerate = envirCam_checkPerson(&change_1, &change_2);
          //  printf("decelerate ! %f \n" , decelerate);
            if(speed_ratio >= 0){ 
                speed_ratio = max_check(speed_ratio);
                cur_speed = speed_ratio - decelerate;
             //   printf("cur_speed ! %f \n" , cur_speed);
                pca9685->setPWM(0, 0, (float)duty_cycle*cur_speed);
                pca9685->setPWM(1, 0, (float)duty_cycle*cur_speed);
            }else{
                speed_ratio = min_check(speed_ratio);
            }
        }
        printf("quit motor control\n");
    }
    pca9685->closePCA9685();
    unmap_all();
}
bool is_closePerson(){
    float direct_pix_boundary = 1000;
    const float width = 1280;
    const float height = 720;
    bool isClose = false;
 
    pthread_mutex_lock(&mute_objection_center);
    if(g_object_width > direct_pix_boundary){
        isClose = true;
        //printf("close person, speed = 0\n");
    }
    pthread_mutex_unlock(&mute_objection_center);

    return isClose;
}
//window size init inside(change manually)
Direction trackObject_direction(){
    float x_centralPoint;
    float y_centralPoint;
    float direct_pix_boundary = 350;
    const float width = 1280;
    const float height = 720;
    Direction enum_dir = none;
    
    pthread_mutex_lock(&mute_objection_center);
    if(g_objection_center != zoreTuple ){
        x_centralPoint = std::get<0>(g_objection_center);
        y_centralPoint = std::get<1>(g_objection_center);
       // printf("x_centralPoint: %02f\n",x_centralPoint) ;
      //  printf("y_centralPoint: %02f\n",y_centralPoint) ;
    }
    pthread_mutex_unlock(&mute_objection_center);
    if(g_object_width != 0 ){
        // in right of window
        if(x_centralPoint < 1280 && x_centralPoint > (width - direct_pix_boundary)){
            enum_dir = right;
        }//in lift of window
        else if(x_centralPoint > 0 && x_centralPoint < direct_pix_boundary){
            enum_dir = left;
        }else{
            enum_dir = center;
        }
    }
    return enum_dir;
}
float envirCam_checkPerson(int* change_1, int* change_2){
    float decelerate = 0;
    pthread_mutex_lock(&mute_pi_person);
    if(g_detecting_person == 1){
        decelerate = 0.6;
        
        if(*change_2 == 1){
            printf("g_detecting_person: %d\n",g_detecting_person) ;
            *change_1 = 1;
            *change_2 = 0;
        }
    }else{
        decelerate = 0 ;
        
        if(*change_1 == 1){
            printf("g_detecting_person: %d\n",g_detecting_person) ;
            *change_2 = 1;
            *change_1 = 0;
        }
    }
    pthread_mutex_unlock(&mute_pi_person);
    return decelerate;
}
void* pwmctl_forward(void* ) {
    PCA9685 *pca9685 = new PCA9685() ;
    int err = pca9685->openPCA9685();
    setmap_all();
    if (err < 0){
        printf("Error: %d", pca9685->error);
    } 
    else{
        printf("PCA9685 Device Address: 0x%02X\n",pca9685->kI2CAddress) ;
        pca9685->setAllPWM(0,0) ;
        pca9685->reset() ;
        pca9685->setPWMFrequency(60) ;
        float speed_ratio = 0;
        float decelerate = 0;
        float cur_speed = 0;
        /*
            crictial section persoin decelerate
        */
        set_forward_GPIO_valt();
        int change_1 = 1;
        int change_2 = 1;
        while(true)
        {
            char input_c = getkey();
            if(input_c == 'w'){
                speed_ratio += 0.1;
                printf("speed_ratio: %02f\n",speed_ratio) ;
            }
            if(input_c == 's'){
                speed_ratio -= 0.1;
                printf("speed_ratio: %02f\n",speed_ratio) ;
            }
            if(input_c == 'q'){
                pca9685->setPWM(0, 0, (float)0);
                pca9685->setPWM(1, 0, (float)0);
                break;
            }
            decelerate = envirCam_checkPerson(&change_1, &change_2);

            if(speed_ratio >= 0){ 
                speed_ratio = max_check(speed_ratio);
                cur_speed = speed_ratio - decelerate;
                //printf("speed_ratio : %f\n",speed_ratio);

                pca9685->setPWM(0, 0, (float)duty_cycle*cur_speed);
                pca9685->setPWM(1, 0, (float)duty_cycle*cur_speed);
            }else{
                speed_ratio = min_check(speed_ratio);
            }
        }
        printf("quit motor control\n") ;
    }
    pca9685->closePCA9685();
    unmap_all();
}
int pwmctl_byChar() {
    PCA9685 *pca9685 = new PCA9685() ;
    int err = pca9685->openPCA9685();
    char N1[] = "200";
    char N2[] = "38";
    char N3[] = "76";
    char N4[] = "12";
    set_GPIO(N1);
    set_GPIO(N2);
    set_GPIO(N3);
    set_GPIO(N4);
    if (err < 0){
        printf("Error: %d", pca9685->error);
    } else {
        printf("PCA9685 Device Address: 0x%02X\n",pca9685->kI2CAddress) ;
        pca9685->setAllPWM(0,0) ;
        pca9685->reset() ;
        pca9685->setPWMFrequency(60) ;
        // 27 is the ESC key
        printf("Hit ESC(q) key to exit\n");
        //&& getkey() != 27

        float speed_ratio = 0;
        float left_speed_ratio = 0;
        float right_speed_ratio = 0;
        float l_pos_speed_ratio = 0;
        float r_pos_speed_ratio = 0;
 
        while(pca9685->error >= 0 ){
            char input_c = getkey();
            if(input_c == 'w'){
                speed_ratio += 0.1;
                left_speed_ratio = 0;
                right_speed_ratio = 0;
            }
            if(input_c == 's'){
                speed_ratio -= 0.1;
                left_speed_ratio = 0;
                right_speed_ratio = 0;
            }
            if(input_c == 'a'){
                left_speed_ratio += 0.1;
                right_speed_ratio = 0;
            }
            if(input_c == 'd'){
                left_speed_ratio = 0;
                right_speed_ratio += 0.1;
            }
            if(speed_ratio < 0){
                if(speed_ratio <= -1){
                    speed_ratio = -1;
                }
                set_GPIO_value_high(N2);
                set_GPIO_value_low(N1);
                set_GPIO_value_high(N4);
                set_GPIO_value_low(N3);
                l_pos_speed_ratio = speed_ratio*((float)-1); 
                r_pos_speed_ratio = speed_ratio*((float)-1);
                pca9685->setPWM(0, 0, (float)duty_cycle*l_pos_speed_ratio);
                pca9685->setPWM(1, 0, (float)duty_cycle*r_pos_speed_ratio);
                printf(" speed : %f\n", speed_ratio);
            }
            else if(speed_ratio >= 0){
                if(speed_ratio >= 1){
                    speed_ratio = 1;
                }
                set_GPIO_value_high(N1);
                set_GPIO_value_low(N2);
                set_GPIO_value_high(N3);
                set_GPIO_value_low(N4);
                l_pos_speed_ratio = speed_ratio; 
                r_pos_speed_ratio = speed_ratio;
                l_pos_speed_ratio += left_speed_ratio;
                r_pos_speed_ratio += right_speed_ratio;
                l_pos_speed_ratio = max_check(l_pos_speed_ratio);
                r_pos_speed_ratio = max_check(r_pos_speed_ratio);
                if(left_speed_ratio != 0){
                    r_pos_speed_ratio = 0;                
                }
                if(right_speed_ratio != 0){
                    l_pos_speed_ratio = 0;
                }    
                pca9685->setPWM(0, 0, (float)duty_cycle*l_pos_speed_ratio);
                pca9685->setPWM(1, 0, (float)duty_cycle*r_pos_speed_ratio);
                printf(" l_pos_speed_ratio : %f\n", l_pos_speed_ratio);
                printf(" r_pos_speed_ratio : %f\n", r_pos_speed_ratio);
            }
            if(input_c == 'q'){
                break;
            }
        }
    }
    pca9685->closePCA9685();
    unmap_GPIO(N1);
    unmap_GPIO(N2);
    unmap_GPIO(N3);
    unmap_GPIO(N4);
}

void setmap_all(){
    set_GPIO(N1);
    set_GPIO(N2);
    set_GPIO(N3);
    set_GPIO(N4);
}
void unmap_all(){
    unmap_GPIO(N1);
    unmap_GPIO(N2);
    unmap_GPIO(N3);
    unmap_GPIO(N4);
}
void set_back_GPIO_valt(){
    set_GPIO_value_high(N2);
    set_GPIO_value_low(N1);
    set_GPIO_value_high(N4);
    set_GPIO_value_low(N3);
}
void set_forward_GPIO_valt(){
    set_GPIO_value_high(N1);
    set_GPIO_value_low(N2);
    set_GPIO_value_high(N3);
    set_GPIO_value_low(N4);
}
void set_lift_GPIO_valt(){
    set_GPIO_value_low(N1);
    set_GPIO_value_low(N2);
    set_GPIO_value_high(N3);
    set_GPIO_value_low(N4);
}
void set_right_GPIO_valt(){
    set_GPIO_value_high(N1);
    set_GPIO_value_low(N2);
    set_GPIO_value_low(N3);
    set_GPIO_value_low(N4);
}


char getkey() {
    int character;
    char input_char;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    input_char = getc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return input_char;
}
void set_GPIO(const char* BCM_num){
    sleep(1);
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd == -1){
        perror("Unable to open /sys/class/gpio/export");
        exit(1);
    }
    if(write(fd, BCM_num, strlen(BCM_num)) != strlen(BCM_num)){
        perror("Error writing /sys/class/gpio/export");
        exit(1);
    }
    close(fd);
    
    sleep(1);
    char dir[125] = "/sys/class/gpio/gpio";
    strcat(dir, BCM_num);
    strcat(dir, "/direction");
    fd = open(dir, O_WRONLY);
    if(fd == -1){
        perror("Unable to open /sys/class/gpio/direction");
        exit(1);
    }
    if(write(fd, "out", 3) != 3){
        perror("Error writing /sys/class/gpio/direction");
        exit(1);
    }
    close(fd); 
}
void set_GPIO_value_high(const char* BCM_num){
    char dir[125] = "/sys/class/gpio/gpio";
    strcat(dir, BCM_num);
    strcat(dir, "/value");

    int fd = open(dir, O_WRONLY);
    if(fd == -1){
        perror("Unable to open /sys/class/gpio/value");
        exit(1);
    }
    if(write(fd, "1", 1) != 1){
        perror("Error writing /sys/class/gpio/value");
        exit(1);
    }
    close(fd);
}
void set_GPIO_value_low(const char* BCM_num){
    char dir[125] = "/sys/class/gpio/gpio";
    strcat(dir, BCM_num);
    strcat(dir, "/value");

    int fd = open(dir, O_WRONLY);
    if(fd == -1){
        perror("Unable to open /sys/class/gpio/value");
        exit(1);
    }
    if(write(fd, "0", 1) != 1){
        perror("Error writing /sys/class/gpio/value");
        exit(1);
    }
    close(fd);
}
void unmap_GPIO(const char* BCM_num){
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if(fd == -1){
        perror("Unable to open /sys/class/gpio/unexport");
        exit(1);
    }
    if(write(fd, BCM_num, strlen(BCM_num)) != strlen(BCM_num)){
        perror("Error writing /sys/class/gpio/unexport");
        exit(1);
    }
    close(fd);
}
float max_check(float value){
    if(value > 1){
        return 1;      
    }else{
        return value;
    }
}
float min_check(float value){
    if(value < -1){
        return -1;      
    }else{
        return value;
    }
}
bool check_PCA9685Error(int errorCode, PCA9685 *pca9685){
    if (errorCode < 0){
        printf("Error: %d", pca9685->error);
        return false;
    }
    return true; 
}

