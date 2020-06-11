/*

Copyright (C) 2020 Alluris GmbH & Co. KG <weber@alluris.de>

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

string liballuris::err_msg (int err)
{
  string ret = "";
  switch (err)
    {
    case LIBUSB_ERROR_NOT_FOUND:
    case LIBUSB_ERROR_NO_DEVICE:
    case LIBUSB_ERROR_PIPE:
    case LIBUSB_ERROR_IO:
      ret =  gettext (
               "Keine Verbindung zum TTT.\n"
               "Bitte überprüfen Sie die USB-Leitung zum PC und stellen Sie sicher, dass\n"
               "das TTT eingeschaltet ist. Drücken Sie die Start/Stop Taste am TTT falls\n"
               "die USB-Verbindung erst nach der Initialisierung eingesteckt wurde.");
      break;
    default:
      break;
    }

  return ret;
}

#define ERR_PREFIX(e) liballuris_error_name (e) << " in " << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << "):"
#define RUNTIME_ERROR(e, msg) if(e) { std::ostringstream tmp_err; tmp_err << ERR_PREFIX(e) << msg << "\n\n" << err_msg(e); throw runtime_error (tmp_err.str ());}

liballuris::liballuris ()
{
  cout << "liballuris c'tor" << endl;

  int r = libusb_init (&usb_ctx);
  RUNTIME_ERROR(r, "c'tor  libusb_init");

  r = liballuris_open_device (usb_ctx, NULL, &usb_h);
  RUNTIME_ERROR(r, "c'tor  liballuris_open_device");

  r = libusb_claim_interface (usb_h, 0);
  RUNTIME_ERROR(r, "c'tor libusb_claim_interface");
}

liballuris::liballuris (string serial)
{
  cout << "liballuris c'tor with serial = " << serial << endl;

  int r = libusb_init (&usb_ctx);
  RUNTIME_ERROR(r,"c'tor libusb_init");

  r = liballuris_open_device (usb_ctx, serial.c_str(), &usb_h);
  RUNTIME_ERROR(r,"c'tor open_device. Couldn't connect to FMI-S/B device with given serial.");

  r = libusb_claim_interface (usb_h, 0);
  RUNTIME_ERROR(r,"c'tor libusb_claim_interface");
}

liballuris::~liballuris ()
{
  cout << "liballuris d'tor" << endl;
  liballuris_clear_RX (usb_h, 500);
  liballuris_clear_RX (usb_h, 500);
  libusb_release_interface (usb_h, 0);
  libusb_close (usb_h);
}

void liballuris::set_debug_level (int l)
{
  liballuris_set_debug_level (l);
}

string liballuris::get_serial_number ()
{
  char tmp_buf[SERIAL_LEN];
  int r = liballuris_get_serial_number (usb_h, tmp_buf, SERIAL_LEN);
  RUNTIME_ERROR(r,"get_serial_number");
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

  if (cal_date == 0xFFFF)
    return "not calibrated";

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

  std::cout << int(tmp_buf[0]) << std::endl;
  if (tmp_buf[0] == -1)
    return "not calibrated";

  return tmp_buf;
}

double liballuris::get_uncertainty ()
{
  double uncertainty;
  int r = liballuris_get_uncertainty (usb_h, &uncertainty);
  RUNTIME_ERROR(r,"");
  if (isnan (uncertainty))
    uncertainty = 1;
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

int liballuris::get_upper_limit ()
{
  int limit;
  int r = liballuris_get_upper_limit (usb_h, &limit);
  RUNTIME_ERROR(r,"");
  return limit;
}

int liballuris::get_lower_limit ()
{
  int limit;
  int r = liballuris_get_lower_limit (usb_h, &limit);
  RUNTIME_ERROR(r,"");
  return limit;
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
  int tmp = round (level * 100);
  int r = liballuris_set_peak_level (usb_h, tmp);
  RUNTIME_ERROR(r,"");
}

double liballuris::get_peak_level ()
{
  int level;
  int r = liballuris_get_peak_level (usb_h, &level);
  RUNTIME_ERROR(r,"");
  return level / 100.0;
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

void liballuris::set_memory_mode (liballuris_memory_mode mode)
{
  int r = liballuris_set_mem_mode (usb_h, mode);
  RUNTIME_ERROR(r,"");
}

vector<int> liballuris::read_memory ()
{
  vector<int> ret;
  int num = 0;
  int r = liballuris_get_mem_count (usb_h, &num);
  RUNTIME_ERROR(r,"liballuris::read_memory get_mem_count");

  for (int k=0; k < num; ++k)
    {
      int value = 0;
      r = liballuris_read_memory (usb_h, k, &value);
      RUNTIME_ERROR(r,"liballuris::read_memory");
      ret.push_back (value);
    }
  return ret;
}

void liballuris::clear_memory ()
{
  int r = liballuris_delete_memory (usb_h);
  RUNTIME_ERROR(r,"clear_memory");
}

void liballuris::set_data_ratio (int decimate)
{
  int r = liballuris_set_data_ratio (usb_h, decimate);
  RUNTIME_ERROR(r,"set_data_ratio");
}

int liballuris::get_pos_peak ()
{
  int peak;
  int r = liballuris_get_pos_peak (usb_h, &peak);
  RUNTIME_ERROR(r,"get_pos_peak");
  return peak;
}

int liballuris::get_neg_peak ()
{
  int peak;
  int r = liballuris_get_neg_peak (usb_h, &peak);
  RUNTIME_ERROR(r,"get_neg_peak");
  return peak;
}

void liballuris::clear_pos_peak ()
{
  int r = liballuris_clear_pos_peak (usb_h);
  RUNTIME_ERROR(r,"clear_pos_peak");
}

void liballuris::clear_neg_peak ()
{
  int r = liballuris_clear_neg_peak (usb_h);
  RUNTIME_ERROR(r,"clear_neg_peak");
}

string liballuris::get_variant ()
{
  char tmp_buf[VARIANT_LEN];
  int r = liballuris_get_variant (usb_h, tmp_buf, VARIANT_LEN);
  RUNTIME_ERROR(r,"get_variant");
  return tmp_buf;
}

string liballuris::get_firmware ()
{
  char tmp_buf[SERIAL_LEN];
  int r = liballuris_get_firmware (usb_h, 0, tmp_buf, SERIAL_LEN);
  RUNTIME_ERROR(r,"get_firmware");
  return tmp_buf;
}

liballuris_state liballuris::read_state ()
{
  struct liballuris_state state;
  int r = liballuris_read_state (usb_h, &state, 2000);  //FIXME: in gadc habe ich 3000 als timeout
  RUNTIME_ERROR(r,"read_state");
  return state;
}

void liballuris::start_motor_reference_run (char start)
{
  int r = liballuris_start_motor_reference_run (usb_h, start);
  RUNTIME_ERROR(r,"start_motor_reference_run");
}

void liballuris::set_motor_disable (bool state)
{
  int r = liballuris_set_motor_disable (usb_h, state);
  RUNTIME_ERROR(r,"set_motor_disable");
}

bool liballuris::get_motor_enable ()
{
  char enabled;
  int r = liballuris_get_motor_enable (usb_h, &enabled);
  RUNTIME_ERROR(r,"get_motor_enable");
  return enabled == 1;
}

void liballuris::set_buzzer_motor (bool state)
{
  int r = liballuris_set_buzzer_motor (usb_h, state);
  RUNTIME_ERROR(r,"set_buzzer_motor");
}

bool liballuris::get_buzzer_motor ()
{
  char state;
  int r = liballuris_get_buzzer_motor (usb_h, &state);
  RUNTIME_ERROR(r,"get_buzzer_motor");
  return state != 0;
}

void liballuris::set_motor_start (uint8_t start)
{
  int r = liballuris_set_motor_start (usb_h, start);
  RUNTIME_ERROR(r,"set_motor_start");
}

void liballuris::set_motor_stopp (uint8_t state)
{
  int r = liballuris_set_motor_stopp (usb_h, state);
  RUNTIME_ERROR(r,"set_motor_stopp");
}
