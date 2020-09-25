// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright Drew Noakes 2013-2016

#include "joystick.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include "unistd.h"
#include <pthread.h>
extern int g_ps4_forward;
extern int g_ps4_back;
extern int g_ps4_left;
extern int g_ps4_right;
extern int g_ps4_stop;

extern int g_ps4_b_left;
extern int g_ps4_b_down;
extern int g_ps4_b_right;

extern pthread_mutex_t mute_ps4_direct_ctl;

Joystick::Joystick()
{
  openPath("/dev/input/js0");
}

Joystick::Joystick(int joystickNumber)
{
  std::stringstream sstm;
  sstm << "/dev/input/js" << joystickNumber;
  openPath(sstm.str());
}

Joystick::Joystick(std::string devicePath)
{
  openPath(devicePath);
}

Joystick::Joystick(std::string devicePath, bool blocking)
{
  openPath(devicePath, blocking);
}

void Joystick::openPath(std::string devicePath, bool blocking)
{
  // Open the device using either blocking or non-blocking
  _fd = open(devicePath.c_str(), blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK);
}

bool Joystick::sample(JoystickEvent* event)
{
  int bytes = read(_fd, event, sizeof(*event)); 

  if (bytes == -1)
    return false;

  // NOTE if this condition is not met, we're probably out of sync and this
  // Joystick instance is likely unusable
  return bytes == sizeof(*event);
}

bool Joystick::isFound()
{
  return _fd >= 0;
}

Joystick::~Joystick()
{
  close(_fd);
}

std::ostream& operator<<(std::ostream& os, const JoystickEvent& e)
{
  os << "type=" << static_cast<int>(e.type)
     << " number=" << static_cast<int>(e.number)
     << " value=" << static_cast<int>(e.value);
  return os;
}

void* ps4Controller_run(void* ){
  Joystick joystick("/dev/input/js0");
  // Ensure that it was found and that we can use it
  if (!joystick.isFound())
  {
    printf("open failed.\n");
    exit(1);
  }
  while (true)
  {
    // Restrict rate
    usleep(1000);

    // Attempt to sample an event from the joystick
    JoystickEvent event;
    if (joystick.sample(&event))
    {
      if (event.isButton())
      {
        printf("Button %u is %s\n",event.number,event.value == 0 ? "up" : "down");
        if(event.number == 0 && event.value != 0){
            //left button down 
            g_ps4_b_left = 1;
        }
        if(event.number == 1 && event.value != 0){
            //down button  down
            g_ps4_b_down = 1;
        }
        if(event.number == 2 && event.value != 0){
            //right button  down
            g_ps4_b_right = 1;
        }
      }
      else if (event.isAxis())
      {
        pthread_mutex_lock(&mute_ps4_direct_ctl);      
        printf("Axis %u is at position %d\n", event.number, event.value);
        if(event.value == 0){
            g_ps4_forward = 0;
            g_ps4_back = 0;
            g_ps4_left = 0;
            g_ps4_right = 0;
            g_ps4_stop = 1;
        }
        else{
            if(event.number == 0){
                if(event.value > 0){
                    g_ps4_forward = 0;
                    g_ps4_back = 0;
                    g_ps4_left = 1;
                    g_ps4_right = 0;
                    g_ps4_stop = 0;
                }else if(event.value < 0){
                    g_ps4_forward = 0;
                    g_ps4_back = 0;
                    g_ps4_left = 0;
                    g_ps4_right = 1;
                    g_ps4_stop = 0;
                }
            }else if(event.number == 1){
                if(event.value > 0){
                    g_ps4_forward = 0;
                    g_ps4_back = 1;
                    g_ps4_left = 0;
                    g_ps4_right = 0;
                    g_ps4_stop = 0;
                }else if(event.value < 0){
                    g_ps4_forward = 1;
                    g_ps4_back = 0;
                    g_ps4_left = 0;
                    g_ps4_right = 0;
                    g_ps4_stop = 0;
                }
            }
        }
        pthread_mutex_unlock(&mute_ps4_direct_ctl);
      }
    }
  }
}


