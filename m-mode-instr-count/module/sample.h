#ifndef _MODULE_H
#define _MODULE_H

#include <stddef.h>

#define MODULE_DEVICE_NAME "sample"
#define MODULE_DEVICE_PATH "/dev/" MODULE_DEVICE_NAME


#define MODULE_IOCTL_MAGIC_NUMBER (long)0xf00f00

#define MODULE_IOCTL_CMD_INC \
  _IOR(MODULE_IOCTL_MAGIC_NUMBER, 1, size_t)
//       ^-- command name         ^-- unique command ID
#define MODULE_IOCTL_CMD_INC2 \
  _IOR(MODULE_IOCTL_MAGIC_NUMBER, 2, size_t)
//       ^-- command name         ^-- unique command ID
  
#endif
