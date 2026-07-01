#include <libevdev/libevdev.h>
#include <stdbool.h>
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

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
    printf("Keybord: %s\n", kb_path);
  }

  //open up the kb input event file
  int fd = open(kb_path, O_RDONLY);

  //buffer to read input events
  struct input_event event;


  //main logger loop
  while (true) {
    read(fd, &event, sizeof(struct input_event));
    if(event.type == EV_KEY){
      printf("code: %d\tvalue: %d\n", event.code, event.value);
    }
  }
}
