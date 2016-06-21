#include <stdio.h>
#include "liballuris.h"

#define POLL_PACKET_SIZE 19

int main()
{
  libusb_context* usb_ctx;
  libusb_device_handle* usb_h;

  int r = libusb_init (&usb_ctx);
  fprintf (stderr, "libusb_init:                   %s\n", liballuris_error_name (r));

  r = liballuris_open_device (usb_ctx, NULL, &usb_h);
  fprintf (stderr, "liballuris_open_device:        %s\n", liballuris_error_name (r));

  r = libusb_claim_interface (usb_h, 0);
  fprintf (stderr, "libusb_claim_interface:        %s\n", liballuris_error_name (r));
  
  r = liballuris_start_measurement (usb_h);
  fprintf (stderr, "liballuris_start_measurement:  %s\n", liballuris_error_name (r));

  sleep (1);
  r = liballuris_cyclic_measurement (usb_h, 1, 19);
  fprintf (stderr, "liballuris_cyclic_measurement: %s\n", liballuris_error_name (r));
  
  // poll
  int tempx[POLL_PACKET_SIZE];
  size_t act;

  int k;
  for (k=0; k < 100; ++k)
    {
      r = liballuris_poll_measurement_no_wait (usb_h, tempx, POLL_PACKET_SIZE, &act);
      // we sometimes expect (and ignore) LIBUSB_ERROR_TIMEOUT here
      if (r && r != LIBUSB_ERROR_TIMEOUT)
        fprintf (stderr, "Error: %s\n", liballuris_error_name (r));
      else
        printf ("values polled = %i\n", act);
      usleep (1e4);
    }
    
  //std::vector<int> samples;
  //for (unsigned int k=0; k < act; ++k)
  //  samples.push_back (tempx[k]);
    
  r = liballuris_cyclic_measurement (usb_h, 0, 19);
  fprintf (stderr, "liballuris_cyclic_measurement: %s\n", liballuris_error_name (r));

  r = liballuris_stop_measurement (usb_h);
  fprintf (stderr, "liballuris_stop_measurement:   %s\n", liballuris_error_name (r));
}
