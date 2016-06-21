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
  code {setlocale (LC_ALL, "");
//bindtextdomain("ttt","/usr/share/locale");
bindtextdomain("ttt","./po");
textdomain ("ttt");} {}
  Fl_Window mainwin {
    label {TTT quick check config} open
    xywh {2076 275 375 555} type Double color 40 visible
  } {
    Fl_Group {} {
      label {Prüfung} open
      xywh {5 5 363 250} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_nominal {
        label {Nominalwert [Nm]}
        xywh {195 40 55 25} minimum -50 maximum 50 step 0.1
      }
      Fl_Value_Input vi_test_object_accuracy {
        label {Höchstzulässige Abweichung [%]}
        callback {double val = o->value ();

vi_upper_limit->value (vi_nominal->value () * (1 + val/100.0));
vi_lower_limit->value (vi_nominal->value () * (1 - val/100.0));}
        xywh {195 73 55 25} align 132 minimum 0.1 maximum 10 step 0.1 value 6
      }
      Fl_Value_Input vi_lower_limit {
        label {Unterer Grenzwert [Nm]}
        xywh {195 107 55 25} minimum -50 maximum 50 step 0.01
      }
      Fl_Value_Input vi_upper_limit {
        label {Oberer Grenzwert [Nm]}
        xywh {195 141 55 25} minimum -50 maximum 50 step 0.01
      }
      Fl_Value_Input vi_peak_level {
        label {Peakdetektion [%]}
        callback {if (o->value () < 50)
  o->color (FL_RED);
else
  o->color (FL_WHITE);
o->redraw ();}
        tooltip {Peakdetektion < 50% ist nicht empfehlenswert} xywh {195 175 55 25} align 132 minimum 10 maximum 99 step 1 value 90
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

    if (vi_nominal->value () > 0)
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
    al.set_memory_mode (LIBALLURIS_MEM_MODE_SINGLE);
    al.set_unit (LIBALLURIS_UNIT_N);
    al.set_peak_level (vi_peak_level->value() / 100.0);
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }
mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {30 215 315 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button {} {
        label {3%}
        callback {vi_test_object_accuracy->value (3);
vi_test_object_accuracy->do_callback ();}
        xywh {265 73 45 25} box GLEAM_THIN_UP_BOX
      }
      Fl_Button {} {
        label {6%}
        callback {vi_test_object_accuracy->value (6);
vi_test_object_accuracy->do_callback ();}
        xywh {309 73 45 25} box GLEAM_THIN_UP_BOX
      }
    }
    Fl_Group {} {
      label Messwerte open selected
      xywh {5 265 363 285} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
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
      fl_message ("Keine Werte im Gerätespeicher");
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
        xywh {200 300 148 35} box GLEAM_THIN_UP_BOX
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
    //quick_tbl->hide ();
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }
mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {200 340 148 35} box GLEAM_THIN_UP_BOX
      }
      Fl_Table quick_tbl {open
        xywh {20 298 155 157} box GTK_THIN_DOWN_FRAME
        class quick_check_table
      } {}
      Fl_Value_Output vo_mean {
        label {Mittelwert [Nm]}
        xywh {20 500 110 30} box DOWN_BOX color 49 selection_color 49 labelfont 0 labelsize 14 labelcolor 0 align 5 minimum 0 maximum 1 step 0.001 textsize 14 textcolor 0
      }
      Fl_Value_Output vo_std {
        label {Standardabweichung [Nm]}
        xywh {140 500 110 30} box DOWN_BOX color 49 selection_color 49 labelfont 0 labelsize 14 labelcolor 0 align 5 minimum 0 maximum 1 step 0.001 textsize 14 textcolor 0
      }
    }
  }
  code {//quick_tbl->hide ();} {}
}
