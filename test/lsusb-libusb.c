/* lsusb-libusb.c: Portable lsusb for all libusb-supported OSes
 * Copyright (c) 2009, Kirn Gill II <segin2005@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  libusb_device **devs, *dev;
  int ret, count, i;
  struct libusb_device_descriptor desc;

  ret = libusb_init(NULL);
  if (ret < 0)
    return ret;

  count = libusb_get_device_list(NULL, &devs);
  if (count < 0)
    return count;

  for(i = 0; i < count; i ++)
    {
      dev = devs[i];
      ret = libusb_get_device_descriptor(dev, &desc);

      if (ret < 0)
        {
          fprintf(stderr, "Failed to get device descriptor!\n");
          return(-1);
        }
      printf("Bus %03d Device %03d %04x:%04x\n",
             libusb_get_bus_number(dev),
             libusb_get_device_address(dev),
             desc.idVendor, desc.idProduct);
    };

  libusb_free_device_list(devs, 1);

  libusb_exit(NULL);
  return(0);
}
