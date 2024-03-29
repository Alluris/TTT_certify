/*

Copyright (C) 2015-2020 Alluris GmbH & Co. KG <weber@alluris.de>

This file is part of liballuris.

Liballuris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Liballuris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with liballuris. See ../COPYING.LESSER
If not, see <http://www.gnu.org/licenses/>.

*/

/*!
 * \file liballuris.c
 * \brief Implementation of generic Alluris device driver
*/

#include "liballuris.h"

int liballuris_debug_level;

// minimum length of "in" is 2 bytes
static unsigned short char_to_uint16 (unsigned char* in)
{
  unsigned short ret = 0;
  memcpy (&ret, in, 2);
  return ret;
}

// minimum length of "in" is 3 bytes
static int char_to_uint24 (unsigned char* in)
{
  int ret = 0;
  memcpy (&ret, in, 3);
  return ret;
}

// minimum length of "in" is 3 bytes
static int char_to_int24 (unsigned char* in)
{
  int ret = char_to_uint24 (in);
  if (ret > 8388607)     // 2**23-1
    ret = ret- 16777216; // 2**24
  return ret;
}

/*!
 * Returns a constant NULL-terminated string with the ASCII name of a libusb
 * or liballuris error code. The caller must not free() the returned string.
 *
 * \param error_code The \ref liballuris_error code to return the name of error.
 * \returns The error name, or the string **UNKNOWN** if the value of
 * error_code is not a known error code.
 */
const char * liballuris_error_name (int error_code)
{

  if (error_code < 0)
    {
      enum libusb_error error = (enum libusb_error) error_code;
      return libusb_error_name (error);
    }
  else
    {
      enum liballuris_error error = (enum liballuris_error) error_code;
      switch (error)
        {
        case LIBALLURIS_SUCCESS:
          return "LIBALLURIS_SUCCESS";
        case LIBALLURIS_MALFORMED_REPLY:
          return "LIBALLURIS_MALFORMED_REPLY";
        case LIBALLURIS_DEVICE_BUSY:
          return "LIBALLURIS_DEVICE_BUSY";
        case LIBALLURIS_TIMEOUT:
          return "LIBALLURIS_TIMEOUT";
        case LIBALLURIS_OUT_OF_RANGE:
          return "LIBALLURIS_OUT_OF_RANGE";
        case LIBALLURIS_PARSE_ERROR:
          return "LIBALLURIS_PARSE_ERROR";
        default:
          return "**UNKNOWN LIBALLURIS_ERROR**";
        }
    }
  return "**UNKNOWN**";
}

/*!
 * Convert enum liballuris_unit to string
 *
 * \param unit as liballuris_unit
 * \returns N, cN, kg, g, lb, oz or **UNKNOWN UNIT**
 */
const char * liballuris_unit_enum2str (enum liballuris_unit unit)
{
  switch (unit)
    {
    case LIBALLURIS_UNIT_N:
      return "N";
    case LIBALLURIS_UNIT_cN:
      return "cN";
    case LIBALLURIS_UNIT_kg:
      return "kg";
    case LIBALLURIS_UNIT_g:
      return "g";
    case LIBALLURIS_UNIT_lb:
      return "lb";
    case LIBALLURIS_UNIT_oz:
      return "oz";
    }
  return "**UNKNOWN UNIT**";
}

/*!
 * Convert string to enum liballuris_unit
 *
 * \param str which can be N, cN, kg, g, lb, oz
 * \returns enum liballuris_unit or -1 if unknown unit
 */
enum liballuris_unit liballuris_unit_str2enum (const char *str)
{
  if (! strcmp (str, "N"))
    return LIBALLURIS_UNIT_N;
  else if (! strcmp (str, "cN"))
    return LIBALLURIS_UNIT_cN;
  else if (! strcmp (str, "kg"))
    return LIBALLURIS_UNIT_kg;
  else if (! strcmp (str, "g"))
    return LIBALLURIS_UNIT_g;
  else if (! strcmp (str, "lb"))
    return LIBALLURIS_UNIT_lb;
  else if (! strcmp (str, "oz"))
    return LIBALLURIS_UNIT_oz;
  else
    return (enum liballuris_unit) -1;
}

//! Internal function to print send or receive buffers
static void print_buffer (unsigned char* buf, int len)
{
  int k;
  for (k=0; k < len; ++k)
    {
      fprintf (stderr, "0x%02x", buf[k]);
      if (k < (len - 1))
        fprintf (stderr, ", ");
    }
  fprintf (stderr, "\n");
}

//! Internal send and receive wrapper around libusb_interrupt_transfer
static int liballuris_interrupt_transfer (libusb_device_handle* dev_handle,
    const char* funcname,
    unsigned char *out_buf,
    int send_len,
    unsigned int send_timeout,
    unsigned char *in_buf,
    int reply_len,
    unsigned int receive_timeout)
{
  int actual;
  int r = 0;
  struct timeval t1, t2;

  if (reply_len > DEFAULT_RECV_BUF_LEN)
    {
      fprintf (stderr, "Error: Reply len %i > receive buffer len %i. This looks like a programming error.\n", reply_len, DEFAULT_RECV_BUF_LEN);
      exit (-1);
    }

  if (send_len > 0)
    {
      // check length in out_buf
      assert (out_buf[1] == send_len);

      if (liballuris_debug_level)
        gettimeofday (&t1, NULL);

      r = libusb_interrupt_transfer (dev_handle, (0x1 | LIBUSB_ENDPOINT_OUT), out_buf, send_len, &actual, send_timeout);

      if (liballuris_debug_level)
        {
          gettimeofday (&t2, NULL);
          double diff = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1.0e6;
          fprintf (stderr, "DEBUG-INFO: %s send  took %f s\n", funcname, diff);
        }

      if (liballuris_debug_level > 1 && r == LIBUSB_SUCCESS)
        {
          fprintf (stderr, "DEBUG-INFO: %s sent %2i/%2i bytes: ", funcname, actual, send_len);
          print_buffer (out_buf, actual);
        }

      if (r != LIBUSB_SUCCESS || actual != send_len)
        {
          fprintf(stderr, "Write error in '%s': '%s', wrote %i of %i bytes.\n", funcname, libusb_error_name(r), actual, send_len);
          return r;
        }
    }

  if (reply_len > 0)
    {
      unsigned char tmp_in_buf[DEFAULT_RECV_BUF_LEN];
      // ID_SAMPLE bis zu 3 mal ignorieren, falls ein anderes Kommando gesendet wurde
      int sample_ignore_cnt = 3;
      do
        {
          //~ if (sample_ignore_cnt < 3)
            //~ printf ("retry...\n");
          if (liballuris_debug_level)
            gettimeofday (&t1, NULL);

          r = libusb_interrupt_transfer (dev_handle, 0x81 | LIBUSB_ENDPOINT_IN, tmp_in_buf, DEFAULT_RECV_BUF_LEN, &actual, receive_timeout);

          if (liballuris_debug_level)
            {
              gettimeofday (&t2, NULL);
              double diff = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1.0e6;
              fprintf (stderr, "DEBUG-INFO: %s reply took %f s\n", funcname, diff);
            }

          if (liballuris_debug_level > 1 && r == LIBUSB_SUCCESS)
            {
              fprintf (stderr, "DEBUG-INFO: %s recv %2i/%2i bytes: ", funcname, actual, DEFAULT_RECV_BUF_LEN);
              print_buffer (in_buf, actual);
            }

          if (r != LIBUSB_SUCCESS)
            {
              if (r == LIBUSB_ERROR_OVERFLOW)
                {
                  if (liballuris_debug_level)
                    fprintf (stderr, "DEBUG-INFO: LIBUSB_ERROR_OVERFLOW in '%s': expected max. %i bytes but got more.\n", funcname, DEFAULT_RECV_BUF_LEN);

                  // Attention! You can't rely that data was written in in_buf
                  // See: http://libusb.sourceforge.net/api-1.0/libusb_packetoverflow.html
                }
              else
                fprintf(stderr, "Read error in '%s': '%s', tried to read %i, got %i bytes.\n", funcname, libusb_error_name(r), DEFAULT_RECV_BUF_LEN, actual);

              return r;
            }
        }
      // ID_SAMPLE bis zu sample_ignore_cnt mal igorieren/verwerfen wenn nicht gewünscht (falls streaming aktiv ist)
      while (sample_ignore_cnt-- > 0 && tmp_in_buf[0] == 0x02 && send_len > 0);

      if (send_len > 0              // nur dann ist out_buf[0] valide
          && (tmp_in_buf[0] != out_buf[0] ||  tmp_in_buf[1] != reply_len))
        {
          fprintf(stderr, "Error: Malformed reply. Check physical connection and EMI.\n");
          fprintf(stderr, "(send_cmd=0x%02X != recv_cmd=0x%02X) || (recv_len=%i != reply_len=%i),\n", out_buf[0], tmp_in_buf[0], tmp_in_buf[1], reply_len);

          return LIBALLURIS_MALFORMED_REPLY;
        }

      memcpy (in_buf, tmp_in_buf, reply_len);
  }
  return r;
}

/****************************************************************************************/

/*!
 * \brief List accessible alluris devices
 *
 * The product field in alluris_device_description is filled via the USB descriptor.
 * After this the device is opened and the serial_number is read from the device.
 * Thus this function only lists devices where the application has sufficient rights to open
 * and read from the device. Check permissions if a device isn't returned.
 *
 * The retrieved list has to be freed with \ref liballuris_free_device_list before the application exits.
 * \param[in] ctx pointer to libusb context
 * \param[out] alluris_devs pointer to storage for the device list
 * \param[in] length number of elements in alluris_devs
 * \param[in] read_serial try to read the serial from devices
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_free_device_list
 */
int liballuris_get_device_list (libusb_context* ctx, struct alluris_device_description* alluris_devs, size_t length, char read_serial)
{
  size_t k = 0;
  for (k=0; k<length; ++k)
    alluris_devs[k].dev = NULL;

  size_t num_alluris_devices = 0;
  libusb_device **devs;
  if (liballuris_debug_level)
    fprintf (stderr, "DEBUG-INFO: Searching for compatible USB devices...\n");

  ssize_t cnt = libusb_get_device_list (ctx, &devs);
  if (cnt > 0)
    {
      libusb_device *dev;
      int i = 0;
      while ((dev = devs[i++]) != NULL)
        {
          struct libusb_device_descriptor desc;
          int r = libusb_get_device_descriptor (dev, &desc);
          if (r < 0)
            {
              fprintf (stderr, "failed to get device descriptor: %s", libusb_error_name(r));
              continue;
            }

          if (liballuris_debug_level)
            fprintf (stderr, "DEBUG-INFO: desc.idVendor = 0x%04X, desc.idProduct = 0x%04X", desc.idVendor, desc.idProduct);

          //check for compatible devices
          // FMIS or TTT
          if (desc.idVendor == 0x04d8 && (desc.idProduct == 0xfc30 || desc.idProduct == 0xf25e) )
            {

              if (liballuris_debug_level)
                fprintf (stderr, " (compatible)\n");

              alluris_devs[num_alluris_devices].dev = dev;

              //open device
              libusb_device_handle* h;
              r = libusb_open (dev, &h);
              if (r == LIBUSB_SUCCESS)
                {
                  r = libusb_claim_interface (h, 0);
                  if (r == LIBUSB_SUCCESS)
                    {

                      if(desc.iProduct)
                        r = libusb_get_string_descriptor_ascii(h, desc.iProduct, (unsigned char*)alluris_devs[num_alluris_devices].product, sizeof (alluris_devs[0].product));
                      else
                        strncpy (alluris_devs[num_alluris_devices].product, "No product information available", sizeof (alluris_devs[0].product));

                      if (read_serial)
                        {
                          // get serial number from device
                          r = liballuris_get_serial_number (h, alluris_devs[num_alluris_devices].serial_number, sizeof (alluris_devs[0].serial_number));
                          if (r == LIBALLURIS_DEVICE_BUSY)
                            // measurement is running, serial cannot be read
                            strcpy (alluris_devs[num_alluris_devices].serial_number, "*BUSY*");
                        }

                      num_alluris_devices++;
                      libusb_release_interface (h, 0);
                      libusb_close (h);
                    }
                  else if (r == LIBUSB_ERROR_BUSY)
                    {
                      // On GNU/Linux this is raised if the device is already in use
                      // for example from another application
                    }
                  else
                    fprintf (stderr, "liballuris_get_device_list: Couldn't claim interface: %s\n", libusb_error_name(r));

                }
              else
                fprintf (stderr, "liballuris_get_device_list: Couldn't open device: %s\n", libusb_error_name(r));
            }
          else
            {
              if (liballuris_debug_level)
                fprintf (stderr, "\n");
              // unref non Alluris device
              libusb_unref_device (dev);
            }

          if (num_alluris_devices == length)
            break; // maximum number of devices reached
        }
    }
  libusb_free_device_list (devs, 0);
  return num_alluris_devices;
}

/*!
 * \brief Free list filled from get_alluris_device_list
 */
void liballuris_free_device_list (struct alluris_device_description* alluris_devs, size_t length)
{
  size_t k = 0;
  for (k=0; k < length; ++k)
    if (alluris_devs[k].dev)
      {
        libusb_unref_device (alluris_devs[k].dev);
        alluris_devs[k].dev = NULL;
      }
}

void liballuris_print_device_list (FILE *sink, libusb_context* ctx)
{
  // list accessible devices and exit
  // FIXME: document that a running measurement prohibits reading the serial_number

  struct alluris_device_description alluris_devs[MAX_NUM_DEVICES];
  ssize_t cnt = liballuris_get_device_list (ctx, alluris_devs, MAX_NUM_DEVICES, 1);

  int k;
  fprintf (sink, "#Num; Bus; Dev; Product;                   Serial\n");
  for (k=0; k < cnt; k++)
    fprintf (sink, " %03i; %03d; %03d; %-25s; %s\n", k+1,
             libusb_get_bus_number(alluris_devs[k].dev),
             libusb_get_device_address(alluris_devs[k].dev),
             alluris_devs[k].product,
             alluris_devs[k].serial_number);

  if (!cnt)
    fprintf (stderr, "Error: No accessible device found\n");

  // free device list
  liballuris_free_device_list (alluris_devs, MAX_NUM_DEVICES);
}

/*!
 * \brief Open device with specified serial_number or the first available if NULL
 * \param[in] ctx pointer to libusb context
 * \param[in] serial_number of device or NULL
 * \param[out] h storage for handle to communicate with the device
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_open_device (libusb_context* ctx, const char* serial_number, libusb_device_handle** h)
{
  libusb_device *dev = NULL;
  struct alluris_device_description alluris_devs[MAX_NUM_DEVICES];
  int cnt = liballuris_get_device_list (ctx, alluris_devs, MAX_NUM_DEVICES, (serial_number != NULL));

  if (liballuris_debug_level)
    fprintf (stderr, "DEBUG-INFO: liballuris_open_device: found %i device(s)\n", cnt);

  if (cnt >= 1)
    {
      if (serial_number == NULL)
        dev = alluris_devs[0].dev;
      else
        for (int k = 0; k < cnt; k++)
          if (! strncmp (serial_number, alluris_devs[k].serial_number, sizeof (alluris_devs[0].serial_number)))
            dev = alluris_devs[k].dev;
    }

  if (! dev)
    //no device found
    return LIBUSB_ERROR_NOT_FOUND;

  int ret = libusb_open (dev, h);
  liballuris_free_device_list (alluris_devs, MAX_NUM_DEVICES);
  return ret;
}


/*!
 * \brief Open device with specified bus and device id.
 * \param[in] ctx pointer to libusb context
 * \param[in] bus id of device
 * \param[in] device id of device
 * \param[out] h storage for handle to communicate with the device
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_open_device_with_id (libusb_context* ctx, int bus, int device, libusb_device_handle** h)
{
  libusb_device *dev = NULL;
  struct alluris_device_description alluris_devs[MAX_NUM_DEVICES];
  int cnt = liballuris_get_device_list (ctx, alluris_devs, MAX_NUM_DEVICES, 0);

  if (cnt >= 1)
    {
      for (int k = 0; k < cnt; k++)
        if (   libusb_get_bus_number (alluris_devs[k].dev) == bus
               && libusb_get_device_address (alluris_devs[k].dev) == device)
          dev = alluris_devs[k].dev;
    }

  if (! dev)
    //no device found
    return LIBUSB_ERROR_NOT_FOUND;

  int ret = libusb_open (dev, h);
  liballuris_free_device_list (alluris_devs, MAX_NUM_DEVICES);
  return ret;
}

/*!
 * \brief Check if a device is already connected (h != 0) and connect else
 *
 * If serial_or_bus_id contains one dot (.), it's considered as a serial number
 * for example L.12345, if it contains a comma (,) serial_or_bus_id is considered to
 * be bus_id,device_id pair.
 * \param[in] ctx pointer to libusb context
 * \param[in] serial_or_bus_id serial or bus_id,device_id or NULL
 * \return 0 if successful else \ref liballuris_error
 */

int liballuris_open_if_not_opened (libusb_context* ctx, const char* serial_or_bus_id, libusb_device_handle** h)
{
  //printf ("liballuris_open_if_not_opened h=%x\n", *h);

  int r = 0;
  if (! *h)
    {
      if (! serial_or_bus_id) //connect to first available device
        {
          r = liballuris_open_device (ctx, serial_or_bus_id, h);
          if (r)
            {
              fprintf (stderr, "Error: Couldn't open device: %s\n", liballuris_error_name (r));
            }
        }
      else //bus,device or serial
        {
          // decide whether it's a serial number or a bus,device pair
          const char* pc = strchr (serial_or_bus_id, ',');
          if (pc)
            {
              // split "Bus,Device"
              char *endptr, *endptr2 = NULL;
              int bus = strtol (serial_or_bus_id, &endptr, 10);
              if (endptr == serial_or_bus_id)
                {
                  fprintf (stderr, "Error: Couldn't find bus_id in '%s'\n", serial_or_bus_id);
                  return LIBALLURIS_PARSE_ERROR;
                }
              if (*endptr != ',')
                {
                  fprintf (stderr, "Error: Wrong delimiter '%c', please use ',' instead\n", *endptr);
                  return LIBALLURIS_PARSE_ERROR;
                }
              int device = strtol (++endptr, &endptr2, 10);
              if (endptr2 == endptr)
                {
                  fprintf (stderr, "Error: Couldn't find device_id in '%s'\n", serial_or_bus_id);
                  return LIBALLURIS_PARSE_ERROR;
                }

              r = liballuris_open_device_with_id (ctx, bus, device, h);
              if (r)
                {
                  fprintf (stderr, "Error: Couldn't open device with bus=%i and device=%i: %s\n", bus, device, liballuris_error_name (r));
                }
            }
          else // serial
            {
              r = liballuris_open_device (ctx, serial_or_bus_id, h);
              if (r)
                {
                  fprintf (stderr, "Error: Couldn't open device with serial='%s': %s\n", serial_or_bus_id, liballuris_error_name (r));
                }
            }
        }

      if (*h)
        {
          r = libusb_claim_interface (*h, 0);
          if (r)
            {
              fprintf (stderr, "Error: Couldn't claim interface: %s\n", liballuris_error_name (r));
            }
        }
    }
  //printf ("open_if_not_opened h=%x\n", *h);
  return r;
}

/*!
 * \brief Clear receive buffer
 *
 * Try to read 64 bytes from receive endpoint to clear the USB receive queue after some error.
 * There is no error if a timeout occurs.
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] timeout read timeout
 */
void liballuris_clear_RX (libusb_device_handle* dev_handle, unsigned int timeout)
{
  unsigned char data[64];
  int actual;
  int r = libusb_interrupt_transfer (dev_handle, 0x81 | LIBUSB_ENDPOINT_IN, data, 64, &actual, timeout);

  if (liballuris_debug_level)
    fprintf (stderr, "DEBUG-INFO: clear_RX: libusb_interrupt_transfer returned '%s', actual = %i\n", libusb_error_name(r), actual);
}

/*!
 * \brief Query the serial number
 *
 * Return the serial number of the device which is also laser-engraved on the back.
 * For example "P.25412"
 *
 * Query the serial number is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] buf output location for the serial number. Only populated if the return code is 0.
 * \param[in] length length of buffer in bytes
 * \return 0 if successful else \ref liballuris_error. LIBALLURIS_DEVICE_BUSY if measurement is running
 */
int liballuris_get_serial_number (libusb_device_handle *dev_handle, char* buf, size_t length)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 6;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      unsigned short tmp = char_to_uint16 (in_buf + 3);
      if (tmp == 65535)
        return LIBALLURIS_DEVICE_BUSY;
      else
        snprintf (buf, length, "%c.%i", in_buf[5] + 'A', tmp);
    }
  return ret;
}

/*!
 * \brief Query firmware version
 *
 * The returned string in buf is for example "V5.03.009"
 * If the measurement is running, the firmware can't be read from the measurement processor
 * and "V255.255.255" is returned instead.
 *
 * Since firmware V5.05.003 the measurement processor firmware can no longer
 * been read separately and will always return V0.255.255
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] dev 0=USB communication processor, 1=measurement processor
 * \param[out] buf output location for the firmware string. Only populated if the return code is 0.
 * \param[in] length length of buffer in bytes
 * \return 0 if successful else \ref liballuris_error.
 */
int liballuris_get_firmware (libusb_device_handle *dev_handle, int dev, char* buf, size_t length)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  if (dev < 0 || dev > 1)
    return LIBALLURIS_OUT_OF_RANGE;

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = dev;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    snprintf (buf, length, "V%i.%02i.%03i", in_buf[5], in_buf[4], in_buf[3]);
  return ret;
}

/*!
 * \brief Query next calibration date
 *
 * The returned int v is YYMM for example "1502" for February 2015
 * If the measurement is running, the next calibration date can't be read from the measurement processor
 * and LIBALLURIS_DEVICE_BUSY is returned instead.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the next calibration date. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error.
 */
int liballuris_get_next_calibration_date (libusb_device_handle *dev_handle, int* v)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 7;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v = char_to_int24 (in_buf + 3);
      if (*v == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

int liballuris_read_flash (libusb_device_handle *dev_handle, int adr, unsigned short *v)
{
  unsigned char out_buf[4];
  unsigned char in_buf[6];

  out_buf[0] = 0x72;
  out_buf[1] = 4;
  out_buf[2] = adr & 0xFF;
  out_buf[3] = adr >> 8;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v  = in_buf[5] << 8;
      *v += in_buf[4];
    }
  return ret;
}

/*!
 * \brief Query calibration date
 * Since firmware 5.04.005
 * The returned int v is days since year 2000
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the calibration date. May be partially populated if the return code is != 0.
 * \return 0 if successful else \ref liballuris_error.
 */
int liballuris_get_calibration_date (libusb_device_handle *dev_handle, unsigned short* v)
{
  // see liballuris_get_calibration_number for flash organisation
  return liballuris_read_flash (dev_handle, 0, v);
}

//! Since firmware 5.04.005
int liballuris_get_calibration_number (libusb_device_handle *dev_handle, char* buf, size_t length)
{
  // PIC flash organisation
  // word adr; word value (16bit)
  // 0       ; cal_date, days since 1.1.2000, unsigned short
  // 1..4    ; uncertainty, double
  // 5..24   ; calibration_number as char[40]

  if (length > 40)
    length = 40;
  // needs to be multiple of 2bytes (words)
  if (length % 2)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned int k, ret;
  unsigned short *p = (unsigned short *) buf;
  for (k=0; k<length/2; ++k)
    {
      ret = liballuris_read_flash (dev_handle, k + 5, p++);
      if (ret != LIBALLURIS_SUCCESS)
        break;
    }
  return ret;
}

//! Since firmware 5.04.005
int liballuris_get_uncertainty (libusb_device_handle *dev_handle, double* v)
{
  // see liballuris_get_calibration_number for flash organisation
  int k, ret;
  unsigned short *p = (unsigned short *) v;
  for (k=0; k<4; ++k)
    {
      ret = liballuris_read_flash (dev_handle, k + 1, p++);
      if (ret != LIBALLURIS_SUCCESS)
        break;
    }
  return ret;
}

/*!
 * \brief Query the number of digits for the interpretation of the raw fixed-point numbers
 *
 * All liballuris_get_* functions return fixed-point numbers or strings. This function queries the number
 * of digits after the radix point. For example if \ref liballuris_get_value returns 123 and digits returns 1
 * the real value is 12.3. Use \ref liballuris_get_unit to get a meaningful measurement.
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the returned number of digits. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_value
 */
int liballuris_get_digits (libusb_device_handle *dev_handle, int* v)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 3;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v = char_to_int24 (in_buf + 3);
      if (*v == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

/*!
 * \brief Query the variant
 *
 * Supported devices:
 * 0x0000: FMI-S10
 * 0x0001: FMI-S20
 * 0x0002: FMI-S30
 * 0x0003: FMI-W30
 * 0x0004: FMI-W40
 * 0x0005: FMI-S50
 * 0x0006: FMI-W20
 * 0x0007: FMT-W10
 * 0x0010: FMI-B10
 * 0x0011: FMI-B20
 * 0x0012: FMI-B30
 * 0x0015: FMI-B50
 * 0x0020: FMT-315
 * 0x0040: CTT-200
 * 0x0080: CTT-300
 * 0x0100: TTT-200
 * 0x0200: TTT-300
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the returned variant. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_value
 */
int liballuris_get_variant (libusb_device_handle *dev_handle, char* buf, size_t length)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 4;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      int v = char_to_int24 (in_buf + 3);
      if (v == -1)
        return LIBALLURIS_DEVICE_BUSY;

      switch (v)
        {
        case LIBALLURIS_VARIANT_S10:
          snprintf (buf, length, "FMI-S10");
          break;
        case LIBALLURIS_VARIANT_S20:
          snprintf (buf, length, "FMI-S20");
          break;
        case LIBALLURIS_VARIANT_S30:
          snprintf (buf, length, "FMI-S30");
          break;
        case LIBALLURIS_VARIANT_W30:
          snprintf (buf, length, "FMT-W30");
          break;
        case LIBALLURIS_VARIANT_W40:
          snprintf (buf, length, "FMT-W40");
          break;
        case LIBALLURIS_VARIANT_S50:
          snprintf (buf, length, "FMI-S50");
          break;
        case LIBALLURIS_VARIANT_W20:
          snprintf (buf, length, "FMI-W20");
          break;
        case LIBALLURIS_VARIANT_W10:
          snprintf (buf, length, "FMT-W10");
          break;
        case LIBALLURIS_VARIANT_B10:
          snprintf (buf, length, "FMI-B10");
          break;
        case LIBALLURIS_VARIANT_B20:
          snprintf (buf, length, "FMI-B20");
          break;
        case LIBALLURIS_VARIANT_B30:
          snprintf (buf, length, "FMI-B30");
          break;
        case LIBALLURIS_VARIANT_B50:
          snprintf (buf, length, "FMI-B50");
          break;
        case LIBALLURIS_VARIANT_FMT_315:
          snprintf (buf, length, "FMT-315");
          break;
        case LIBALLURIS_VARIANT_CTT_200:
          snprintf (buf, length, "CTT-200");
          break;
        case LIBALLURIS_VARIANT_CTT_300:
          snprintf (buf, length, "CTT-300");
          break;
        case LIBALLURIS_VARIANT_TTT_200:
          snprintf (buf, length, "TTT-200");
          break;
        case LIBALLURIS_VARIANT_TTT_300:
          snprintf (buf, length, "TTT-300");
          break;
        default:
          snprintf (buf, length, "unknown");
          break;
        }
    }
  return ret;
}

/*!
 * \brief Query the resolution
 *
 * Since firmware 5.04.005
 *
 * The resolution as floating point number is 10^(-get_digits) * get_resolution.
 * Possible values for resolution are 1, 2, 5.
 *
 * Example: get_unit returns LIBALLURIS_UNIT_N, get_digit returns 2,
 * get_resolution returns 5 -> real resolution of 0.05N
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the returned resolution. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_digits
 */

int liballuris_get_resolution (libusb_device_handle *dev_handle, int* v)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 16;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v = char_to_int24 (in_buf + 3);
      if (*v == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

/*!
 * \brief Query the maximum force of the device
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] fmax output location for the returned max force. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_F_max (libusb_device_handle *dev_handle, int* fmax)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *fmax = char_to_int24 (in_buf + 3);
      if (*fmax == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

/*!
 * \brief Query the current measurement value
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] value output location for the measurement value. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_value (libusb_device_handle *dev_handle, int* value)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x46;
  out_buf[1] = 3;
  out_buf[2] = 3;
  // worst execution time = 0.466s
  // when P13=1Hz (effectively 2Hz) and mode=0 (10Hz)
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *value = char_to_int24 (in_buf + 3);
  return ret;
}

/*!
 * \brief Query positive peak value
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] peak output location for the peak value. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_pos_peak (libusb_device_handle *dev_handle, int* peak)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x46;
  out_buf[1] = 3;
  out_buf[2] = 4;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *peak = char_to_int24 (in_buf + 3);
  return ret;
}

/*!
 * \brief Query negative peak value
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] peak output location for the peak value. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_neg_peak (libusb_device_handle *dev_handle, int* peak)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x46;
  out_buf[1] = 3;
  out_buf[2] = 5;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *peak = char_to_int24 (in_buf + 3);
  return ret;
}

/*!
 * \brief Query the current state of the device
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] state output location for the state. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_read_state (libusb_device_handle *dev_handle, struct liballuris_state* state, unsigned int timeout)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x46;
  out_buf[1] = 3;
  out_buf[2] = 2;

  // worst execution time = 0.47s
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), timeout);
  if (ret == LIBALLURIS_SUCCESS)
    {
      if (   in_buf[3] == 0xff
             && in_buf[4] == 0xff
             && in_buf[5] == 0xff)
        return LIBALLURIS_DEVICE_BUSY;

      union __liballuris_state__ tmp;
      tmp._int = char_to_int24 (in_buf + 3);
      *state = tmp.bits;
    }
  return ret;
}

//! Print state to stdout
void liballuris_print_state (struct liballuris_state state)
{
  printf ("[%c] upper limit exceeded\n",               (state.upper_limit_exceeded)? 'X': ' ');
  printf ("[%c] lower limit underrun\n",               (state.lower_limit_underrun)? 'X': ' ');
  printf ("[%c] peak  mode active\n",                  (state.some_peak_mode_active)? 'X': ' ');
  printf ("[%c] peak+ mode active\n",                  (state.peak_plus_active)? 'X': ' ');
  printf ("[%c] peak- mode active\n",                  (state.peak_minus_active)? 'X': ' ');
  printf ("[%c] Store to memory in progress\n",        (state.mem_running)? 'X': ' ');
  printf ("[%c] overload (abs(F) > 150%%)\n",          (state.overload)? 'X': ' ');
  printf ("[%c] fracture detected (only W20/W40)\n",   (state.fracture)? 'X': ' ');
  printf ("[%c] mem active (P21=1 or P21=2)\n",        (state.mem_active)? 'X': ' ');
  printf ("[%c] mem-conti (store with displayrate)\n", (state.mem_conti)? 'X': ' ');
  printf ("[%c] grenz_option\n",                       (state.grenz_option)? 'X': ' ');
  printf ("[%c] measurement running\n",                (state.measuring)? 'X': ' ');
}

/*!
 * \brief Enable or disable cyclic measurements
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] enable
 * \param[in] length 1..19
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_cyclic_measurement (libusb_device_handle *dev_handle, char enable, size_t length)
{
  if (length < 1 || length > 19)
    {
      fprintf (stderr, "Error: packetlen %zu out of range 1..19.\n", length);
      return LIBALLURIS_OUT_OF_RANGE;
    }

  unsigned char out_buf[4];

  out_buf[0] = 0x01;
  out_buf[1] = 4;
  out_buf[2] = (enable)? 2:0;
  out_buf[3] = length;

  //printf ("liballuris_cyclic_measurement enable=%i\n", enable);
  int ret;
  if (enable)
    {
      unsigned char in_buf[4];
      ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                           out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                           in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
    }
  else
    {
      ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                           out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT, NULL, 0, 0);
      liballuris_clear_RX (dev_handle, 100);
      liballuris_clear_RX (dev_handle, 100);
      liballuris_clear_RX (dev_handle, 100);
    }

  return ret;
}

/*!
 * \brief Poll cyclic measurements
 *
 * Cyclic measurements have to be enabled before with liballuris_cyclic_measurement.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] buf output location for the measurements. Only populated if the return code is 0.
 * \param[in] length of block 1..19, typically the same used with liballuris_cyclic_measurement
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_cyclic_measurement
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_poll_measurement (libusb_device_handle *dev_handle, int* buf, size_t length)
{
  size_t len = 5 + length * 3;
  unsigned char in_buf[len];

  /* Increased receive timeout:
   * The sampling frequency can be selected between 10Hz and 900Hz
   * Therefore the maximum delay until one measurement completes is 1/10Hz = 100ms.
   * Since the block size can be up to 19 (see liballuris_cyclic_measurement) the typically delay
   * in 10Hz mode and blocksize 19 is >1.9s
   *
   * Test case: "gadc --stop --set-mode 0 --start -s 19"
  */

  // worst execution time = 2.4s
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__, NULL, 0, 0, in_buf, len, 3600);
  size_t k;
  for (k=0; k<length; k++)
    buf[k] = char_to_int24 (in_buf + 5 + k*3);

  return ret;
}

/*!
 * \brief Poll cyclic measurements without waiting
 *
 * Cyclic measurements have to be enabled before with liballuris_cyclic_measurement.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] buf output location for the measurements. Only populated if the return code is 0.
 * \param[in] length of block 1..19, typically the same used with liballuris_cyclic_measurement
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_cyclic_measurement
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_poll_measurement_no_wait (libusb_device_handle *dev_handle, int* buf, size_t length, size_t *actual_num_values)
{
  int actual=0;
  int r = 0;

  size_t len = 5 + length * 3;
  unsigned char in_buf[len];
  *actual_num_values = 0;
  r = libusb_interrupt_transfer (dev_handle, 0x81 | LIBUSB_ENDPOINT_IN, in_buf, len, &actual, 1);
  //printf ("actual = %i, %s\n", actual, libusb_error_name(r));

  if ((r == LIBUSB_SUCCESS || r == LIBUSB_ERROR_TIMEOUT ) && actual == (int) len)
    {
      size_t k;
      *actual_num_values = (actual - 5) / 3;
      for (k=0; k < (*actual_num_values); k++)
        buf[k] = char_to_int24 (in_buf + 5 + k*3);
    }
  else if (r == LIBUSB_ERROR_TIMEOUT && actual > 0)
    {
      // this isn't expected
      fprintf (stderr, "Error in liballuris_poll_measurement_no_wait: LIBUSB_ERROR_TIMEOUT and actual = %i, len = %zu\n", actual, len);
      fprintf (stderr, "It isn't expected that this could happen. Please file a bug report.\n");
      return r;
    }

  return r;
}

/*!
 * \brief Tare measurement
 *
 * Calculate the mean over several samples and store this internally as offset.
 * This effectively "zeros" the displayed value.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_tare (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x15;
  out_buf[1] = 3;
  out_buf[2] = 0;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Clear the stored positive peak
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_clear_pos_peak (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x15;
  out_buf[1] = 3;
  out_buf[2] = 1;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Clear the stored negative peak
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */

int liballuris_clear_neg_peak (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x15;
  out_buf[1] = 3;
  out_buf[2] = 2;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Start measurement
 *
 * You may have to wait up to 500ms until you can read stable values with
 * liballuris_get_value() due to "auto tare" if enabled.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_stop_measurement
 */
int liballuris_start_measurement (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x1C;
  out_buf[1] = 3;
  out_buf[2] = 1; //start
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);

  if (ret == LIBALLURIS_SUCCESS)
    {
      // WORKAROUND for <= V5.04.010
      // sleep at least 600ms to avoid BUSY from read_state
      usleep (600e3);

      // wait until measurement processor is configured and running
      struct liballuris_state state;
      ret = liballuris_read_state (dev_handle, &state, 3000);
      if (ret)
        return ret;

      if (! state.measuring)
        return LIBALLURIS_TIMEOUT;
    }
  return ret;
}

/*!
 * \brief Stop measurement
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_start_measurement
 */
int liballuris_stop_measurement (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x1C;
  out_buf[1] = 3;
  out_buf[2] = 0; //stop
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);

  if (ret == LIBALLURIS_SUCCESS)
    {
      // wait until measurement processor is stopped
      struct liballuris_state state;
      ret = liballuris_read_state (dev_handle, &state, 3000);
      if (ret)
        return ret;

      if (state.measuring)
        return LIBALLURIS_TIMEOUT;
    }
  return ret;
}

/*!
 * \brief MotorReferenz / ID_MOTOR_REFERENZ
 */
int liballuris_start_motor_reference_run (libusb_device_handle *dev_handle, char start)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x62;
  out_buf[1] = 3;
  out_buf[2] = start;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Disable motor
 *
 * Disables or enables motor functionality
 * Suported devices:
 * FMI-S30, FMI-S50, FMI-B30, FMI-B50 in combination with FMT-200M
 * FMT-W20, FMT-W40
 *
 * Prevents any movement of integrated or external motor until
 * - power restart of device
 * - motor enable is set again
 *
 * Other functionality of the device will not be affected.
 * A running measurement will be stopped by disabling motor.
 * You should start a reference run after eenabling the motor.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] disable = 1 for disabling or 0 for enabling.
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_set_motor_disable (libusb_device_handle *dev_handle, char disable)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x63;
  out_buf[1] = 3;
  out_buf[2] = disable != 0;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Query ID_INFO "Motor Enable", "Motorfreigabe"
 */
int liballuris_get_motor_enable (libusb_device_handle *dev_handle, char *v)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 15;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v = char_to_int24 (in_buf + 3);
      if (*v == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

/*!
 * \brief Set P17 (Buzzer) / P19 (Motor FMT-220M)
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_set_buzzer_motor (libusb_device_handle *dev_handle, char state)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x25;
  out_buf[1] = 3;
  out_buf[2] = state != 0;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Query P17 (Buzzer) / P19 (Motor FMT-220M)
 */
int liballuris_get_buzzer_motor (libusb_device_handle *dev_handle, char *v)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x26;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *v = in_buf[2];
  return ret;
}

/*!
 * \brief MotorStart / ID_MOTOR_START
 * state
 * 0 Start Motor per USB sperren   : Motor starten bei Messung Start
 * 1 Start Motor per USB freigeben : Motor nicht starten bei Messung Start sondern per USB-Befehl
 * 2 Motor starten, falls freigegeben
 */
int liballuris_set_motor_start (libusb_device_handle *dev_handle, char start)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x66;
  out_buf[1] = 3;
  out_buf[2] = start;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief MotorStopp / ID_MOTOR_STOPP
 * state
 * 0 : Keine Auswirkung
 * 1 : Motor stoppen
 */
int liballuris_set_motor_stopp (libusb_device_handle *dev_handle, char state)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x67;
  out_buf[1] = 3;
  out_buf[2] = state;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Set the upper limit
 *
 * Set the upper threshold value for the "LIMIT" symbol on the LCD
 * and the digital limit output. The symbol is visible and the digital output
 * is high if the measured value exceeds this limit.
 *
 * Setting this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] limit value to set in the range -Fmax..+Fmax
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_upper_limit
 * \sa liballuris_set_lower_limit
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_set_upper_limit (libusb_device_handle *dev_handle, int limit)
{
  struct liballuris_state state;
  int ret = liballuris_read_state (dev_handle, &state, 700);
  if (ret)
    return ret;

  if (state.measuring)
    return LIBALLURIS_DEVICE_BUSY;

  unsigned char out_buf[6];
  unsigned char in_buf[6];

  out_buf[0] = 0x18;
  out_buf[1] = 6;
  out_buf[2] = 0; //maximum
  memcpy (out_buf+3, (unsigned char *) &limit, 3);

  // worst execution time = 0.468s
  // Increased receive timeout due to EEPROM write operation
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  return ret;
}

/*!
 * \brief Set the lower limit
 *
 * Set the lower threshold value for the "LIMIT" symbol on the LCD
 * and the digital limit output. The symbol is visible and the digital output
 * is high if the measured value is under this limit.
 *
 * Setting this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] limit value to set in the range -Fmax..+Fmax
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_lower_limit
 * \sa liballuris_set_upper_limit
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_set_lower_limit (libusb_device_handle *dev_handle, int limit)
{
  struct liballuris_state state;
  int ret = liballuris_read_state (dev_handle, &state, 700);
  if (ret)
    return ret;

  if (state.measuring)
    return LIBALLURIS_DEVICE_BUSY;

  unsigned char out_buf[6];
  unsigned char in_buf[6];

  out_buf[0] = 0x18;
  out_buf[1] = 6;
  out_buf[2] = 1; //minimum
  memcpy (out_buf+3, (unsigned char *) &limit, 3);

  // worst execution time = 0.468s
  // Increased receive timeout due to EEPROM write operation
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  return ret;
}

/*!
 * \brief Query the upper limit
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] limit output location for the limit value. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_upper_limit
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_upper_limit (libusb_device_handle *dev_handle, int* limit)
{
  struct liballuris_state state;
  int ret = liballuris_read_state (dev_handle, &state, 700);
  if (ret)
    return ret;

  if (state.measuring)
    return LIBALLURIS_DEVICE_BUSY;

  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x19;
  out_buf[1] = 3;
  out_buf[2] = 0; //maximum
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *limit = char_to_int24 (in_buf + 3);
  return ret;
}

/*!
 * \brief Query the lower limit
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] limit output location for the limit value. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_lower_limit
 * \sa liballuris_get_unit
 * \sa liballuris_get_digits
 */
int liballuris_get_lower_limit (libusb_device_handle *dev_handle, int* limit)
{
  struct liballuris_state state;
  int ret = liballuris_read_state (dev_handle, &state, 700);
  if (ret)
    return ret;

  if (state.measuring)
    return LIBALLURIS_DEVICE_BUSY;

  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x19;
  out_buf[1] = 3;
  out_buf[2] = 1; //minimum
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *limit = char_to_int24 (in_buf + 3);
  return ret;
}

/*!
 * \brief Set measurement mode
 *
 * Setting this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] mode to set.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_mode
 */
int liballuris_set_mode (libusb_device_handle *dev_handle, enum liballuris_measurement_mode mode)
{
  if (mode < 0 || mode > 3)
    {
      fprintf (stderr, "Error: mode %i out of range 0..3\n", mode);
      return LIBALLURIS_OUT_OF_RANGE;
    }

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x04;
  out_buf[1] = 3;
  out_buf[2] = mode;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != mode)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

/*!
 * \brief Query measurement mode
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] mode output location for the mode. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_mode
 */
int liballuris_get_mode (libusb_device_handle *dev_handle, enum liballuris_measurement_mode *mode)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x05;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *mode = (enum liballuris_measurement_mode) in_buf[2];
  return ret;
}

/*!
 * \brief Set memory mode
 *
 * Setting this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] mode to set.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_mem_mode
 */
int liballuris_set_mem_mode (libusb_device_handle *dev_handle, enum liballuris_memory_mode mode)
{
  if (mode < 0 || mode > 3)
    {
      fprintf (stderr, "Error: memory mode %i out of range 0..3\n", mode);
      return LIBALLURIS_OUT_OF_RANGE;
    }

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x1D;
  out_buf[1] = 3;
  out_buf[2] = mode;
  // worst execution time = 0.475s
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != mode)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

/*!
 * \brief Query memory mode
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] mode output location for the mode. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_mem_mode
 */
int liballuris_get_mem_mode (libusb_device_handle *dev_handle, enum liballuris_memory_mode *mode)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x1E;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *mode = (enum liballuris_memory_mode) in_buf[2];

  // workaround for a firmware bug in versions < FIXME: add version number!
  // check if we get a second reply
  int actual;
  int temp_ret = libusb_interrupt_transfer (dev_handle, 0x81 | LIBUSB_ENDPOINT_IN, in_buf, 3, &actual, 100);
  if (temp_ret == LIBALLURIS_SUCCESS)
    {
      // discard first reply
      *mode = (enum liballuris_memory_mode) in_buf[2];
      //fprintf (stderr, "Warning: double answer for get_mem_mode (0x1E)\n");
    }
  else
    // bug is fixed and we got a timeout (no superfluous reply)
    { }

  return ret;
}

/*!
 * \brief Set unit for all liballuris_get_* functions
 *
 * "kg" and "lb" are not available on 5N and 10N devices.
 * "g" and "oz" are not available on devices with fmax > 10N
 * LIBALLURIS_OUT_OF_RANGE is returned if you nevertheless try to do so.
 *
 * Setting this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] unit to set.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_unit
 */
int liballuris_set_unit (libusb_device_handle *dev_handle, enum liballuris_unit unit)
{
  if (unit < 0 || unit > 5)
    {
      fprintf (stderr, "Error: unit %i out of range 0..5\n", unit);
      return LIBALLURIS_OUT_OF_RANGE;
    }

  // F_max dependent mapping
  int fmax;
  int ret = liballuris_get_F_max (dev_handle, &fmax);
  if (ret)
    return ret;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x1A;
  out_buf[1] = 3;

  if (fmax <= 10) //mapping from chapter 3.14.3
    {
      if (unit == 2 || unit == 4) //kg and lb not available on 5N and 10N devices
        return LIBALLURIS_OUT_OF_RANGE;
      if (unit == 3 || unit == 5)
        unit = (enum liballuris_unit)((int)(unit) - 1);
    }
  else if (unit == 1 || unit == 3 || unit == 5) //g and oz not available on devices with fmax > 10N
    return LIBALLURIS_OUT_OF_RANGE;

  out_buf[2] = unit;
  // worst execution time = 0.482s
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != unit)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

/*!
 * \brief Query unit for all liballuris_get_* functions
 *
 * Query this value is only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] unit output location. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_unit
 */
int liballuris_get_unit (libusb_device_handle *dev_handle, enum liballuris_unit *unit)
{
  // F_max dependent mapping
  int fmax;
  int ret = liballuris_get_F_max (dev_handle, &fmax);
  if (ret)
    return ret;

  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x1B;
  out_buf[1] = 2;
  ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                       out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                       in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *unit = (enum liballuris_unit) in_buf[2];

  // mapping from chapter 3.15.3
  if (fmax <= 10 && (*unit == 2 || *unit == 4))
    *unit = (enum liballuris_unit)((int)(*unit) + 1);

  return ret;
}

/*!
 * \brief Set the binary state of the digital outputs
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] v value in the range 0..7 for the three digital outputs
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_digout
 */
int liballuris_set_digout (libusb_device_handle *dev_handle, int v)
{
  if (v < 0 || v > 7) //only 3 bits
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x21;
  out_buf[1] = 3;
  out_buf[2] = v;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);

  if (in_buf[2] != v)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

/*!
 * \brief Query the binary state of the digital outputs
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_set_digout
 */
int liballuris_get_digout (libusb_device_handle *dev_handle, int *v)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x22;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *v = in_buf[2];
  return ret;
}

/*!
 * \brief Query the binary state of the digital input
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_digout
 */
int liballuris_get_digin (libusb_device_handle *dev_handle, int *v)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x27;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *v = in_buf[2];
  return ret;
}

/*!
 * \brief Restore factory defaults
 *
 * Restore factory defaults and set idle mode.
 *
 * Only possible if the measurement is not running,
 * else LIBALLURIS_DEVICE_BUSY is returned.
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_restore_factory_defaults (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x16;
  out_buf[1] = 3;
  out_buf[2] = 1;
  // worst execution time = 2.34s (on TTT)
  // Long receive timeout because device performs many slow EEPROM write operations
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), 3500);
  if (ret == LIBALLURIS_SUCCESS && in_buf[2] == 0xFF)
    ret = LIBALLURIS_DEVICE_BUSY;
  return ret;
}

/*!
 * \brief Power off the device
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_power_off (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[2];

  out_buf[0] = 0x13;
  out_buf[1] = 2;
  return liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
                                        out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
                                        NULL, 0, DEFAULT_RECEIVE_TIMEOUT);
}

/*!
 * \brief Read the measurement memory
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] adr 0..999
 * \param[out] mem_value output location for the stored value. Only populated when the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_get_mem_count
 */
int liballuris_read_memory (libusb_device_handle *dev_handle, int adr, int* mem_value)
{
  if (adr < 0 || adr > 999)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[4];
  unsigned char in_buf[5];

  out_buf[0] = 0x06;
  out_buf[1] = 4;
  out_buf[2] = adr & 0xFF;
  out_buf[3] = (adr >> 8) & 0xFF;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *mem_value = char_to_int24 (in_buf + 2);
  return ret;
}

/*!
 * \brief Delete the measurement memory
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_delete_memory (libusb_device_handle *dev_handle)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x07;
  out_buf[1] = 3;
  out_buf[2] = 1;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  return ret;
}

/*!
 * \brief Query the number of values stored in memory
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] v output location for the number of values. Only populated if the return code is 0.
 * \return 0 if successful else \ref liballuris_error
 * \sa liballuris_read_memory
 */
int liballuris_get_mem_count (libusb_device_handle *dev_handle, int* v)
{
  unsigned char out_buf[3];
  unsigned char in_buf[6];

  out_buf[0] = 0x08;
  out_buf[1] = 3;
  out_buf[2] = 5;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      *v = char_to_int24 (in_buf + 3);
      if (*v == -1)
        return LIBALLURIS_DEVICE_BUSY;
    }
  return ret;
}

/*!
 * \brief Get statistics from measurement memory
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[out] stats output location for statistics. Only populated when the return code is 0.
 * - stats[0] = MAX_PLUS
 * - stats[1] = MIN_PLUS
 * - stats[2] = MAX_MINUS
 * - stats[3] = MIN_MINUS
 * - stats[4] = AVERAGE
 * - stats[5] = VARIANCE (firmware < V5.04.007 fixed 3 digits, newer version use same digits as the other values)
 * \param[in] length of stats buffer. Should be 6
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_get_mem_statistics (libusb_device_handle *dev_handle, int* stats, size_t length)
{
  unsigned char out_buf[2];
  unsigned char in_buf[20];

  out_buf[0] = 0x09;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    {
      unsigned int k;
      for (k=0; k<length; ++k)
        stats[k] = char_to_int24 (in_buf + 2 + k * 3);
    }
  return ret;
}

/*!
 * \brief Simulate keypress
 *
 * \param[in] dev_handle a handle for the device to communicate with
 * \param[in] mask
 * - Bit 0 = S1 key with label (I)
 * - Bit 1 = S2 key with label (S)
 * - Bit 2 = S3 key with label (>0<)
 * - Bit 3 = simulate a long keypress
 * \return 0 if successful else \ref liballuris_error
 */
int liballuris_sim_keypress (libusb_device_handle *dev_handle, unsigned char mask)
{
  if (mask > 0x0F)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x14;
  out_buf[1] = 3;
  out_buf[2] = mask & 0x0F ;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  return ret;
}

// Parameter P7 (only CTT/TTT)
int liballuris_set_peak_level (libusb_device_handle *dev_handle, int v)
{
  if (v < 1 || v > 99)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x31;
  out_buf[1] = 3;
  out_buf[2] = v;
  // worst execution time = 0.484s
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != v)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

// Parameter P7
int liballuris_get_peak_level (libusb_device_handle *dev_handle, int *v)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x32;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *v = in_buf[2];
  return ret;
}

int liballuris_set_autostop (libusb_device_handle *dev_handle, int v)
{
  if (v < 0 || v > 30)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x33;
  out_buf[1] = 3;
  out_buf[2] = v;
  // reply for set_autostop may take up to 500ms, use 1s as timeout
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), 1000);

  if (in_buf[2] != v)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

int liballuris_get_autostop (libusb_device_handle *dev_handle, int *v)
{
  unsigned char out_buf[2];
  unsigned char in_buf[3];

  out_buf[0] = 0x34;
  out_buf[1] = 2;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (ret == LIBALLURIS_SUCCESS)
    *v = in_buf[2];
  return ret;
}

int liballuris_set_key_lock (libusb_device_handle *dev_handle, char active)
{
  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x68;
  out_buf[1] = 3;
  out_buf[2] = active;
  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != active)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

// since firmware 4.02.002/5.02.002 (06/2013)
// Divider for 900Hz or 10Hz base frequency
// v == 0 or 1 disables decimation
int liballuris_set_data_ratio (libusb_device_handle *dev_handle, int v)
{
  if (v < 0 || v > 255)
    return LIBALLURIS_OUT_OF_RANGE;

  unsigned char out_buf[3];
  unsigned char in_buf[3];

  out_buf[0] = 0x30;
  out_buf[1] = 3;
  out_buf[2] = v;

  int ret = liballuris_interrupt_transfer (dev_handle, __FUNCTION__,
            out_buf, sizeof (out_buf), DEFAULT_SEND_TIMEOUT,
            in_buf, sizeof (in_buf), DEFAULT_RECEIVE_TIMEOUT);
  if (in_buf[2] != v)
    return LIBALLURIS_DEVICE_BUSY;

  return ret;
}

void liballuris_set_debug_level (int l)
{
  liballuris_debug_level = l;
}
