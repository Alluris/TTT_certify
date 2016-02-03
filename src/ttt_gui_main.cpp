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
#include "confuse.h"
#include "ttt_gui.h"
#include "ttt.h"

void reset_nominal_torque_color(void*)
{
  vo_nominal_value->color (FL_GRAY);
  vo_nominal_value->redraw ();
}

void update_nominal_torque(double v)
{
  static double old_v = v;
  vo_nominal_value->value (v);

  // highlight vo_nominal_value if value changes
  if (v != old_v)
    {
      vo_nominal_value->color (FL_YELLOW);
      vo_nominal_value->redraw ();
      Fl::add_timeout(3.00, reset_nominal_torque_color);
    }

  old_v = v;

  if (v == 0)
    v = 10;

  dial_torque->minimum (-abs (v) * 135/180);
  dial_torque->maximum (abs (v) * 135/180);
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
  dial_torque->redraw ();
}

void update_peak_torque(double v)
{
  //cout << "update_peak_torque " << v << endl;
  vo_peak_torque->value (v);
}

/*
 * The first char in s controls the color of the instruction text
 * and the direction buttons "turn right", "turn left"
 */
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
    {
      btn_direction_cw->show ();
      btn_direction_ccw->hide ();
    }
  else if (dir == -1)
    {
      btn_direction_cw->hide ();
      btn_direction_ccw->show ();
    }
  else
    {
      btn_direction_cw->hide ();
      btn_direction_ccw->hide ();
    }

  instruction_buff->text (s.c_str ());
}

void update_step(string s, double v)
{
  // Idea, send beep if step makes progress
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

void test_object_table_selected (int id)
{
  printf ("test_object_table_selected id=%i\n", id);
  vi_test_object_id->value(id);
  vi_test_object_id->show ();
  load_test_object(id);
  test_object_win->hide ();
}

void test_person_table_selected (int id)
{
  printf ("test_person_table_selected id=%i\n", id);
  vi_test_person_id->value(id);
  vi_test_person_id->show ();
  load_test_person(id);
  test_person_win->hide ();
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
  if (argc > 3)
    {
      cerr << "Usage: ttt_gui [TEST_OBJECT_RECORD_ID] [SIM_FN]" << endl;
      return -1;
    }

  // read setting with libconfuse
  static char *database = NULL;
  static double start_peak_torque_factor = 0.6;
  static double stop_peak_torque_factor = 0.1;
  static long int initial_test_person_id = 1;
  static long int initial_test_object_id = 1;

  cfg_opt_t opts[] =
  {
    CFG_SIMPLE_STR("database", &database),
    CFG_SIMPLE_FLOAT("start_peak_torque_factor", &start_peak_torque_factor),
    CFG_SIMPLE_FLOAT("stop_peak_torque_factor", &stop_peak_torque_factor),
    CFG_SIMPLE_INT("selected_test_person", &initial_test_person_id),
    CFG_SIMPLE_INT("selected_test_object", &initial_test_object_id),
    CFG_END()
  };
  cfg_t *cfg;

  /* set default value for the server option */
  database = strdup ("ttt_certify.db");

  cfg = cfg_init (opts, 0);
  cfg_parse (cfg, "ttt_gui.conf");

  printf("database: %s\n", database);
  printf("start_peak_torque_factor: %f\n", start_peak_torque_factor);
  printf("stop_peak_torque_factor: %f\n", stop_peak_torque_factor);
  printf("initial_test_person_id: %li\n", initial_test_person_id);
  printf("initial_test_object_id: %li\n", initial_test_object_id);

  int ret;
  setlocale (LC_ALL, "");
  //bindtextdomain("ttt","/usr/share/locale");
  bindtextdomain("ttt","./po");
  textdomain ("ttt");

#ifdef _WIN32
  std::ofstream out("logfiles/ttt_gui.log");
  std::streambuf *coutbuf = std::cout.rdbuf();
  std::cout.rdbuf(out.rdbuf()); //redirect std::cout
#endif

  create_widgets ();

  // search table callbacks
  to->set_select_cb (test_object_table_selected);
  tp->set_select_cb (test_person_table_selected);

  instruction_buff = new Fl_Text_Buffer ();
  td_instruction->buffer (instruction_buff);
  td_instruction->wrap_mode (Fl_Text_Display::WRAP_AT_BOUNDS, 0);

  Fl_Tooltip::delay (0.5);

  mainwin->show ();

  try
    {
      myTTT = new ttt (update_indicated_torque,
                       update_nominal_torque,
                       update_peak_torque,
                       update_instruction,
                       update_step, update_result,
                       database,
                       start_peak_torque_factor,
                       stop_peak_torque_factor,
                       mtable);

      // test_person_table
      tp->connect_DB (database);
      // test_object_table
      to->connect_DB (database);

      if (argc == 3)
        myTTT->connect_measurement_input (argv[2]);
      else
        myTTT->connect_TTT ();

      // die save buttons deaktivieren
      btn_test_person_save->hide ();
      btn_test_object_save->hide ();
      btn_test_object_abort->hide ();
      btn_test_person_abort->hide ();
      btn_direction_cw->hide ();
      btn_direction_ccw->hide ();

      btn_result->hide ();
      vi_single_peak->hide ();
      to_step->hide ();
      step_progress->hide ();
      vo_step_progress->hide ();

      // init test_person with setting in conf
      vi_test_person_id->value (initial_test_person_id);
      vi_test_person_id->do_callback ();

      search_test_object_equipment_nr->value ("*");
      search_test_person_name->value ("*");
      tp->search_name ("*");

      if (argc > 1)
        vi_test_object_id->value (atoi (argv[1]));
      else
        // select initial test_person from config
        vi_test_object_id->value (initial_test_object_id);

      vi_test_object_id->do_callback ();

      load_torque_tester ();

      ret = Fl::run();
    }
  catch (std::runtime_error &e)
    {
      fl_alert (e.what ());
      ret = -1;
    }

  // save settings
  setlocale (LC_ALL, "C");
  if (! ret)
    {
      initial_test_person_id = vi_test_person_id->value ();
      initial_test_object_id = vi_test_object_id->value ();
      FILE *fp = fopen ("ttt_gui.conf", "w");
      cfg_print (cfg, fp);
      fclose (fp);
    }

  cfg_free (cfg);
  free (database);

  delete myTTT;

#ifdef _WIN32
  std::cout.rdbuf(coutbuf); //restore old std::cout
#endif

  cout << "ttt_gui has finished" << endl;
  return ret;
}
