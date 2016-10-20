# data file for the Fltk User Interface Designer (fluid)
version 1.0304
i18n_type 1
i18n_include <libintl.h>
i18n_function gettext
header_name {.h}
code_name {.cxx}
decl {\#include <FL/fl_ask.H>} {public global
}

decl {\#include "liballuris++.h"} {public global
}

decl {\#include "quick_check_table.h"} {public global
}

Function {} {open
} {
  code {\#ifdef _WIN32
  if (argc == 2)
    {
      // create string on heap (putenv doesn't copy the string)
      // never free it! (this sound like a memory leak, I know)
      char *lang_buf = (char *) malloc (50);
      snprintf (lang_buf, 50, "LANG=%s", argv[1]);
      putenv(lang_buf);
      std::cout << "putenv(" << lang_buf << ")" << std::endl;
    }
\#endif

setlocale (LC_ALL, "");
bindtextdomain("ttt","./po");
textdomain ("ttt");} {}
  Fl_Window mainwin {
    label {TTT_Quick-Check V1.02.003} open selected
    xywh {131 138 375 630} type Double color 40 visible
  } {
    Fl_Group {} {
      label {Prüfung} open
      xywh {6 5 363 294} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_nominal {
        label {Nominalwert [Nm]}
        callback {vi_test_object_accuracy->do_callback ();}
        xywh {196 40 55 25} align 132 minimum -50 maximum 50 step 0.1 value 5
      }
      Fl_Value_Input vi_test_object_accuracy {
        label {Höchstzulässige Abweichung [%]}
        callback {double val = o->value ();

vi_nominal->color (FL_WHITE);
vi_nominal->redraw ();
vi_test_object_accuracy->color (FL_WHITE);
vi_test_object_accuracy->redraw ();

vi_upper_limit->value (vi_nominal->value () * (1 + val/100.0));
vi_lower_limit->value (vi_nominal->value () * (1 - val/100.0));
//vi_upper_limit->do_callback ();
get_peak_direction ();}
        xywh {196 75 55 25} align 132 minimum 0.1 maximum 10 step 0.1 value 6
      }
      Fl_Value_Input vi_lower_limit {
        label {Unterer Grenzwert [Nm]}
        callback {get_peak_direction ();

vi_nominal->value (0);
vi_nominal->color (FL_BACKGROUND_COLOR);
vi_nominal->redraw ();
vi_test_object_accuracy->value (0);
vi_test_object_accuracy->color (FL_BACKGROUND_COLOR);
vi_test_object_accuracy->redraw ();}
        xywh {196 111 55 25} align 132 minimum -50 maximum 50 step 0.01
      }
      Fl_Value_Input vi_upper_limit {
        label {Oberer Grenzwert [Nm]}
        callback {vi_lower_limit->do_callback ();}
        xywh {196 147 55 25} align 132 minimum -50 maximum 50 step 0.01
      }
      Fl_Value_Input vi_peak_level {
        label {Peakdetektion [%]}
        callback {if (o->value () < 50)
  o->color (FL_RED);
else
  o->color (FL_WHITE);
o->redraw ();}
        tooltip {Peakdetektion < 50% ist nicht empfehlenswert} xywh {196 183 55 25} align 132 minimum 10 maximum 99 step 1 value 90
      }
      Fl_Value_Input vi_autostop {
        label {Auto-Stopp [s]}
        callback {if (o->value () < 2)
  o->color (FL_RED);
else if (o->value () < 5)
  o->color (FL_YELLOW);
else
  o->color (FL_WHITE);
o->redraw ();}
        xywh {195 219 55 25} align 132 minimum 1 maximum 30 step 1 value 2
      }
      Fl_Button {} {
        label {Konfiguration ins Messgerät schreiben}
        callback {try
  {
    liballuris al;
    mainwin->cursor (FL_CURSOR_WAIT);
    Fl::wait (0);

    cout << "ttt_device::init clear_RX" << endl;
    al.clear_RX (500);
    cout << "ttt_device::init stop streaming and measurement" << endl;
    al.set_cyclic_measurement (0, 19);
    al.stop_measurement ();

    double scale = 1.0 / pow(10, al.get_digits ());

    if (get_peak_direction ())
      {
        al.set_mode (LIBALLURIS_MODE_PEAK_MAX);
        al.set_upper_limit (round (vi_upper_limit->value () / scale));
        al.set_lower_limit (round (vi_lower_limit->value () / scale));
      }
    else
      {
        al.set_mode (LIBALLURIS_MODE_PEAK_MIN);
        al.set_lower_limit (round (vi_upper_limit->value () / scale));
        al.set_upper_limit (round (vi_lower_limit->value () / scale));
      }
    al.set_memory_mode (LIBALLURIS_MEM_MODE_QUICK_CHECK);
    al.set_unit (LIBALLURIS_UNIT_N);
    al.set_peak_level (vi_peak_level->value () / 100.0);
    al.set_autostop (vi_autostop->value ());
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }
mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {15 250 344 40} box GLEAM_THIN_UP_BOX align 128
      }
      Fl_Button {} {
        label {3%}
        callback {vi_test_object_accuracy->value (3);
vi_test_object_accuracy->do_callback ();}
        xywh {266 75 45 25} box GLEAM_THIN_UP_BOX
      }
      Fl_Button {} {
        label {6%}
        callback {vi_test_object_accuracy->value (6);
vi_test_object_accuracy->do_callback ();}
        xywh {310 75 45 25} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_direction_cw {
        label {@+82redo}
        tooltip Drehrichtung xywh {275 105 70 70} box NO_BOX deactivate
      }
      Fl_Button btn_direction_ccw {
        label {@+88undo}
        tooltip Drehrichtung xywh {275 105 70 70} box NO_BOX hide deactivate
      }
    }
    Fl_Group {} {
      label Messwerte open
      xywh {6 301 363 323} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Button {} {
        label {Speicher auslesen}
        callback {try
  {
    liballuris al;
    mainwin->cursor (FL_CURSOR_WAIT);
    Fl::wait (0);

    al.stop_measurement ();
    double scale = 1.0 / pow(10, al.get_digits ());
    quick_tbl->clear ();

    vector<int> mem;
    mem = al.read_memory ();

    cout << "mem.size ()=" << mem.size () << endl;

    if (mem.size () == 0)
      fl_message (gettext ("Keine Werte im Gerätespeicher"));
    else
      {
        double sum = 0;
        int cnt = 0;
        for (unsigned int k=0; k<mem.size (); k+=2)
          {
            cout << "mem k=" << k << " =" << mem[k] << endl;
            quick_tbl->add_measurement (mem[k] * scale);
            sum += mem[k] * scale;
            cnt++;
          }
        double mean = sum / cnt;
        vo_mean->value (mean);

        // nochmal iterieren für Standardabweichung
        double var = 0;
        for (unsigned int k=0; k<mem.size (); k+=2)
          {
            var += pow (mem[k] * scale - mean, 2);
          }
        var /= (cnt - 1);
        vo_std->value (sqrt (var));
        quick_tbl->show ();
      }
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }
mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {186 336 172 44} box GLEAM_THIN_UP_BOX align 128
      }
      Fl_Button {} {
        label {Speicher löschen}
        callback {try
  {
    liballuris al;
    mainwin->cursor (FL_CURSOR_WAIT);
    Fl::wait (0);

    al.clear_memory ();
    quick_tbl->clear ();
    vo_std->value (0);
    vo_mean->value (0);
    //quick_tbl->hide ();
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }
mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {186 391 172 44} box GLEAM_THIN_UP_BOX align 128
      }
      Fl_Table quick_tbl {open
        xywh {21 334 155 157} box GTK_THIN_DOWN_FRAME
        class quick_check_table
      } {}
      Fl_Value_Output vo_mean {
        label {Mittelwert [Nm]}
        xywh {21 560 149 30} box DOWN_BOX align 133 step 0.001
      }
      Fl_Value_Output vo_std {
        label {Standardabweichung [Nm]}
        xywh {190 560 149 30} box DOWN_BOX align 133 step 0.001
      }
    }
  }
  code {//quick_tbl->hide ();
mainwin->show();
return Fl::run();} {}
}

Function {get_peak_direction()} {open return_type bool
} {
  code {bool right = vi_upper_limit->value () > vi_lower_limit->value ();

if (right)
  {
   btn_direction_cw->show ();
   btn_direction_ccw->hide ();
  }
else
  {
   btn_direction_ccw->show ();
   btn_direction_cw->hide ();
  }
  
return right;} {}
}
