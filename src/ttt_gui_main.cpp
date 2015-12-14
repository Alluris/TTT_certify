/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

TTT (tool torque tester) GUI

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  See ../COPYING
If not, see <http://www.gnu.org/licenses/>.

*/

#include <libintl.h>
#include "ttt_gui.h"
#include "ttt.h"

void update_nominal_torque(double v)
{
  vo_nominal_value->value (v);

  if (v == 0)
    v = 10;
  else if (v < 0)
    v = -v;

  dial_torque->minimum (-v * 135/180);
  dial_torque->maximum (v * 135/180);
}

void update_indicated_torque(double v)
{
  double abs_nominal = abs (vo_nominal_value->value ());
  dial_torque->value (v);
  if (abs(v) > abs_nominal)
    dial_torque->color (FL_RED);
  else if (abs(v) > 0.7 * abs_nominal)
    dial_torque->color (FL_YELLOW);
  else
    dial_torque->color (FL_GREEN);
}

void update_peak_torque(double v)
{
  vo_peak_torque->value (v);
}

void update_instruction(string s)
{
  char sp = 0;
  if (s.size() > 0)
    sp = s.at (0);

  if (sp == '*')
    {
      td_instruction->textcolor (FL_RED);
      s = s.substr (1);
    }
  else
    td_instruction->textcolor (FL_BLACK);


  // update direction button
  int dir = 0;
  if (sp == '+')
    dir = 1;
  else if (sp == '-')
    dir = -1;

  if (dir)
    s = s.substr (1);

  if (vo_nominal_value->value () < 0)
    dir = -dir;

  if (dir == 1)
    btn_direction->copy_label ("@+42redo");
  else if (dir == -1)
    btn_direction->copy_label ("@+42undo");
  else
    btn_direction->copy_label ("");

  instruction_buff->text (s.c_str ());
}

void update_step(string s, double v)
{
  // IDEA, send beep if step makes progress
  //fl_beep (FL_BEEP_DEFAULT);

  to_step->value (s.c_str ());
  step_progress->value (v * 100);
  vo_step_progress->value (v * 100);
  /*
    // finished
    if (v >= 1)
      btn_result->show ();
    else
      btn_result->hide ();
  */
}

void update_result(string s)
{
  btn_result->show ();
  if (s.size () && s.at(0) == '*')
    {
      btn_result->color (FL_RED);
      s = s.substr (1);
    }
  else if (s.size () && s.at(0) == '#')
    {
      btn_result->color (FL_YELLOW);
      s = s.substr (1);
    }
  else
    {
      btn_result->color (FL_GREEN);
    }
  btn_result->copy_label (s.c_str ());
}

void run_cb(void*)
{
  if (myTTT->run ())
    Fl::repeat_timeout(0.01, run_cb);
  else
    update_run_activation ();
}

void cairo_box::graphic(cairo_t* cr, double x, double y, double w, double h)
{
  (void) h; //we don't need h here
  cairo_translate (cr, x, y);
  cairo_set_line_width (cr, 0.002);

  cairo_scale (cr, 0.8 * w, 0.8 * w);
  cairo_translate (cr, 0.1, 0.1);

  int id = choice_test_object_type->value();
  cairo_print_id (cr, id);
}

int main(int argc, char **argv)
{
  int ret;
  setlocale (LC_ALL, "");
  //bindtextdomain("ttt","/usr/share/locale");
  bindtextdomain("ttt","./po");
  textdomain ("ttt");

  // FIXME: compare plastic, gtk+, gleam
  // http://www.fltk.org/doc-1.3/classFl.html#a9d070bb1d5a24c28beab53a70af095a4
  //Fl::scheme ("plastic");
  //Fl::scheme ("gleam");

  create_widgets ();

  instruction_buff = new Fl_Text_Buffer ();
  td_instruction->buffer (instruction_buff);
  td_instruction->wrap_mode (Fl_Text_Display::WRAP_AT_BOUNDS, 0);

  Fl_Tooltip::delay (0.5);

  mainwin->show(argc, argv);


  try
    {
      myTTT = new ttt(update_indicated_torque, update_nominal_torque, update_peak_torque, update_instruction, update_step, update_result, "ttt_certify.db");
      myTTT->connect_TTT ();

      // die save buttons deaktivieren
      btn_test_person_save->hide ();
      btn_test_object_save->hide ();
      btn_test_object_abort->hide ();
      btn_test_person_abort->hide ();
      btn_direction->hide ();

      btn_result->hide ();
      vi_single_peak->hide ();
      to_step->hide ();
      step_progress->hide ();
      vo_step_progress->hide ();

      // Anzeigen füllen
      // später hier über libconfuse letzte id für test_person und test_object laden

      // FIXME: vi_test_person_id maximum auf datenbank setzen
      vi_test_person_id->value (1);
      vi_test_person_id->do_callback ();

      vi_test_object_id->value (1);
      vi_test_object_id->do_callback ();

      load_torque_tester ();

      ret = Fl::run();
    }
  catch (std::runtime_error &e)
    {
      fl_alert (e.what ());
    }

  delete myTTT;

  return ret;
}
