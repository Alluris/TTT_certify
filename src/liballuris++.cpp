/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

C++ interface to liballuris.

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

#include "liballuris++.h"

#define ERR_PREFIX(e) liballuris_error_name (e) << " in " << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << "):"
#define RUNTIME_ERROR(e,msg) if(e) { std::ostringstream tmp_err; tmp_err << ERR_PREFIX(r) << msg; throw runtime_error (tmp_err.str ());}

liballuris::liballuris ()
{
  cout << "liballuris c'tor" << endl;

  int r = libusb_init (&usb_ctx);
  RUNTIME_ERROR(r,"c'tor usb_init");

  r = liballuris_open_device (usb_ctx, NULL, &usb_h);
  RUNTIME_ERROR(r,"c'tor open_device");

  r = libusb_claim_interface (usb_h, 0);
  RUNTIME_ERROR(r,"c'tor claim_interface");
}

liballuris::liballuris (string serial)
{
  cout << "liballuris c'tor with serial = " << serial << endl;

  int r = libusb_init (&usb_ctx);
  RUNTIME_ERROR(r,"c'tor usb_init");

  r = liballuris_open_device (usb_ctx, serial.c_str(), &usb_h);
  RUNTIME_ERROR(r,"c'tor open_device");

  r = libusb_claim_interface (usb_h, 0);
  RUNTIME_ERROR(r,"c'tor claim_interface");
}

liballuris::~liballuris ()
{
  cout << "liballuris d'tor" << endl;
  liballuris_clear_RX (usb_h, 500);
  liballuris_clear_RX (usb_h, 500);
  libusb_release_interface (usb_h, 0);
  libusb_close (usb_h);
}

string liballuris::get_serial_number ()
{
  char tmp_buf[SERIAL_LEN];
  int r = liballuris_get_serial_number (usb_h, tmp_buf, SERIAL_LEN);
  RUNTIME_ERROR(r,"");
  return tmp_buf;
}

string liballuris::get_next_calibration_date ()
{
  int next_cal_date;
  int r = liballuris_get_next_calibration_date (usb_h, &next_cal_date);
  RUNTIME_ERROR(r,"");

  // convert YYMM format to string
  int yy = next_cal_date / 100;
  int mm = next_cal_date % 100;
  if (mm < 1 || mm > 12)
    throw runtime_error ("Invalid month in next_calibration date");

  time_t rawtime;
  time ( &rawtime );

  struct tm *timeinfo;
  timeinfo = localtime ( &rawtime );

  timeinfo->tm_sec = 0;
  timeinfo->tm_min = 0;
  timeinfo->tm_hour = 0;
  timeinfo->tm_mday = 1;
  timeinfo->tm_mon = mm - 1;
  timeinfo->tm_year = 100 + yy;

  char buffer[80];
  strftime (buffer, 80, "%Y-%m", timeinfo);
  return buffer;
}

string liballuris::get_calibration_date ()
{
  unsigned short cal_date;
  int r = liballuris_get_calibration_date (usb_h, &cal_date);
  //cout << "liballuris_get_calibration_date cal_date=" << cal_date << endl;
  RUNTIME_ERROR(r,"");

  // convert days since 1.1.2000 to string
  time_t rawtime;
  time ( &rawtime );

  struct tm *timeinfo;
  timeinfo = localtime ( &rawtime );

  timeinfo->tm_sec = 0;
  timeinfo->tm_min = 0;
  timeinfo->tm_hour = 0;
  timeinfo->tm_mday = 1 + cal_date;
  timeinfo->tm_mon = 0;
  timeinfo->tm_year = 100;  // year 2000

  time_t t = mktime (timeinfo);
  timeinfo = localtime (&t);

  char buffer[80];
  strftime (buffer, 80, "%Y-%m-%d", timeinfo);
  return buffer;
}

string liballuris::get_calibration_number ()
{
  char tmp_buf[SERIAL_LEN];
  int r = liballuris_get_calibration_number (usb_h, tmp_buf, SERIAL_LEN);
  RUNTIME_ERROR(r,"");
  return tmp_buf;
}

double liballuris::get_uncertainty ()
{
  double uncertainty;
  int r = liballuris_get_uncertainty (usb_h, &uncertainty);
  RUNTIME_ERROR(r,"");
  return uncertainty;
}

int liballuris::get_digits ()
{
  int digits;
  int r = liballuris_get_digits (usb_h, &digits);
  RUNTIME_ERROR(r,"");
  return digits;
}

int liballuris::get_resolution ()
{
  int resolution;
  int r = liballuris_get_resolution (usb_h, &resolution);
  RUNTIME_ERROR(r,"");
  return resolution;
}

int liballuris::get_F_max ()
{
  int fmax;
  int r = liballuris_get_F_max (usb_h, &fmax);
  RUNTIME_ERROR(r,"");
  return fmax;
}

void liballuris::set_cyclic_measurement (bool enable, int packet_length)
{
  int r = liballuris_cyclic_measurement (usb_h, enable, packet_length);
  RUNTIME_ERROR(r,"");
}

vector<int> liballuris::poll_measurement_no_wait ()
{
  int tempx[POLL_PACKET_SIZE];
  size_t act;
  int r = liballuris_poll_measurement_no_wait (usb_h, tempx, POLL_PACKET_SIZE, &act);
  // we sometimes expect (and ignore) LIBUSB_ERROR_TIMEOUT here
  if (r != LIBUSB_ERROR_TIMEOUT)
    RUNTIME_ERROR(r,"");

  std::vector<int> samples;
  for (unsigned int k=0; k < act; ++k)
    samples.push_back (tempx[k]);
  return samples;
}

void liballuris::tare ()
{
  int r = liballuris_tare (usb_h);
  RUNTIME_ERROR(r,"");
}

void liballuris::start_measurement ()
{
  int r = liballuris_start_measurement (usb_h);
  RUNTIME_ERROR(r,"");
}

void liballuris::stop_measurement ()
{
  int r = liballuris_stop_measurement (usb_h);
  RUNTIME_ERROR(r,"");
}

void liballuris::set_upper_limit (int limit)
{
  int r = liballuris_set_upper_limit (usb_h, limit);
  RUNTIME_ERROR(r,"");
}

void liballuris::set_lower_limit (int limit)
{
  int r = liballuris_set_lower_limit (usb_h, limit);
  RUNTIME_ERROR(r,"");
}

void liballuris::set_mode (enum liballuris_measurement_mode mode)
{
  int r = liballuris_set_mode (usb_h, mode);
  RUNTIME_ERROR(r,"");
}

void liballuris::set_unit (enum liballuris_unit unit)
{
  int r = liballuris_set_unit (usb_h, unit);
  RUNTIME_ERROR(r,"");
}

void liballuris::clear_RX (int timeout)
{
  liballuris_clear_RX (usb_h, timeout);
}

void liballuris::set_peak_level (double level)
{
  int tmp = round (level * 10);
  int r = liballuris_set_peak_level (usb_h, tmp);
  RUNTIME_ERROR(r,"");
}

double liballuris::get_peak_level ()
{
  int level;
  int r = liballuris_get_peak_level (usb_h, &level);
  RUNTIME_ERROR(r,"");
  return level / 10.0;
}

void liballuris::set_autostop (int t)
{
  int r = liballuris_set_autostop (usb_h, t);
  RUNTIME_ERROR(r,"");
}

int liballuris::get_autostop ()
{
  int t;
  int r = liballuris_get_autostop (usb_h, &t);
  RUNTIME_ERROR(r,"");
  return t;
}

void liballuris::set_key_lock (bool active)
{
  int r = liballuris_set_key_lock (usb_h, active);
  RUNTIME_ERROR(r,"");
}

