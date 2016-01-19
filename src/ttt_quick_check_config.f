# data file for the Fltk User Interface Designer (fluid)
version 1.0303 
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
textdomain ("ttt");} {selected
  }
  Fl_Window {} {
    label {TTT quick check config} open
    xywh {2464 349 530 645} type Double visible
  } {
    Fl_Value_Input vi_nominal {
      label {Nominalwert [Nm]}
      xywh {210 20 55 25} minimum -100 maximum 100 step 0.1
    }
    Fl_Value_Input vi_upper_limit {
      label {oberer Grenzwert [Nm]}
      xywh {210 70 55 25} minimum -100 maximum 100 step 0.01
    }
    Fl_Value_Input vi_lower_limit {
      label {unterer Grenzwert [Nm]}
      xywh {210 100 55 25} minimum -100 maximum 100 step 0.01
    }
    Fl_Button {} {
      label {ins Messgerät schreiben}
      callback {try
 {
  liballuris al;

  cout << "ttt_device::init clear_RX" << endl;
  al.clear_RX (500);
  cout << "ttt_device::init stop streaming and measurement" << endl;
  al.set_cyclic_measurement (0, 19);
  al.stop_measurement ();

  double scale = 1.0 / pow(10, al.get_digits ());
  al.set_mode (LIBALLURIS_MODE_PEAK);
  al.set_memory_mode (LIBALLURIS_MEM_MODE_SINGLE);
  al.set_unit (LIBALLURIS_UNIT_N);

  al.set_peak_level (vi_peak_level->value() / 100.0);
  al.set_upper_limit (round (vi_upper_limit->value () / scale));
  al.set_lower_limit (round (vi_lower_limit->value () / scale));

  }
  catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }}
      xywh {150 190 205 35}
    }
    Fl_Button {} {
      label {3%}
      callback {vi_upper_limit->value (vi_nominal->value () * 1.03);
vi_lower_limit->value (vi_nominal->value () * 0.97);}
      xywh {290 83 45 25}
    }
    Fl_Button {} {
      label {6%}
      callback {vi_upper_limit->value (vi_nominal->value () * 1.06);
vi_lower_limit->value (vi_nominal->value () * 0.94);}
      xywh {340 83 45 25}
    }
    Fl_Value_Input vi_peak_level {
      label {Detektionsschwelle [%]}
      xywh {210 130 55 25} minimum 10 maximum 90 step 10 value 20
    }
    Fl_Button {} {
      label {Gerätespeicher auslesen}
      callback {try
{
  liballuris al;

  al.stop_measurement ();
  double scale = 1.0 / pow(10, al.get_digits ());

  vector<int> mem;
  mem = al.read_memory ();

  cout << "mem.size ()=" << mem.size () << endl;

  if (mem.size () == 0)
    fl_message ("Keine Werte im Gerätespeicher");
  else
    {
      double sum = 0;
      for (unsigned int k=0; k<mem.size (); ++k)
       {
         cout << "mem k=" << k << " =" << mem[k] << endl;
         quick_tbl->add_measurement (mem[k] * scale);
         sum += mem[k] * scale;
        }
     vo_mean->value (sum / mem.size ());
     quick_tbl->show ();
     }
  }
  catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }}
      xywh {310 285 205 35}
    }
    Fl_Button {} {
      label {Gerätespeicher löschen}
      callback {try
{
  liballuris al;
  al.clear_memory ();
  quick_tbl->hide ();
  }
  catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }}
      xywh {310 335 205 35}
    }
    Fl_Table quick_tbl {open
      xywh {15 285 195 340}
      class quick_check_table
    } {}
    Fl_Value_Output vo_mean {
      label {Mittelwert [Nm]}
      xywh {435 395 80 30} step 0.01
    }
  }
  code {quick_tbl->hide ();} {}
} 
