# data file for the Fltk User Interface Designer (fluid)
version 1.0304
i18n_type 1
i18n_include <libintl.h>
i18n_function gettext
header_name {.h}
code_name {.cxx}
decl {\#include <FL/fl_ask.H>} {public global
}

decl {\#include "cairo_plot.h"} {public local
}

decl {\#include "ttt_device.h"} {public local
}

decl {\#include "ttt_peak_detector.h"} {public local
}

decl {ttt_device *dev = 0;} {private local
}

decl {bool measuring = 0;} {private local
}

decl {ttt_peak_detector peakd;} {private local
}

decl {vector<double> values;} {private local
}

Function {} {open
} {
  Fl_Window mainwin {
    label {TTT test object param check} open
    xywh {2383 343 1050 705} type Double visible
  } {
    Fl_Box cplot {selected
      xywh {7 8 758 689}
      class cairo_plot
    }
    Fl_Group {} {
      label Messung open
      xywh {775 165 265 115} box GTK_DOWN_BOX align 5
    } {
      Fl_Button btn_start {
        label start
        callback {if (dev)
  {
    dev->start ();
    measuring = 1;
    o->deactivate ();
    btn_stop->activate ();
    values.clear ();
    Fl::add_timeout(0.01, run_cb);
  }}
        xywh {785 180 70 35} box GTK_UP_BOX deactivate
      }
      Fl_Button btn_stop {
        label stop
        callback {if (dev)
  {
    dev->stop ();
    measuring = false;
    o->deactivate ();
    btn_start->activate ();
  }}
        xywh {785 230 70 35} box GTK_UP_BOX deactivate
      }
      Fl_Value_Output vo_value {
        label {Messwert [Nm]}
        xywh {875 195 150 65} align 5 step 0.1 textsize 49
      }
    }
    Fl_Group {} {
      label Peakdetektionsparameter open
      xywh {775 310 265 65} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Input vi_peak1_thres {
        label {Schwellwert Peak 1 [%]}
        tooltip {typsich 80% .. 90%} xywh {970 330 45 25} minimum 30 maximum 99 step 1 value 90
      }
    }
    Fl_Group {} {
      label Statistiken open
      xywh {775 405 265 190} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Output {} {
        label {Peak 1 [Nm]}
        xywh {965 415 55 30} step 0.1
      }
      Fl_Value_Output {} {
        label {min nach Peak 1 [Nm]}
        xywh {965 455 55 30} step 0.1
      }
      Fl_Value_Output {} {
        label {Peak 2 [Nm]}
        xywh {965 495 55 30} step 0.1
      }
    }
    Fl_Group {} {
      label {TTT über USB} open
      xywh {775 25 265 115} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Output vo_mmax {
        label {M_max [Nm]}
        xywh {885 85 50 30} step 0.1
      }
      Fl_Check_Button meas_led {
        xywh {790 35 35 40} type Normal down_box ROUND_DOWN_BOX selection_color 63 labelsize 25 when 0 deactivate
      }
      Fl_Button btn_connect {
        label verbinden
        callback {mainwin->cursor (FL_CURSOR_WAIT);
Fl::wait ();

if (! meas_led->value ())
  {
    try
    {
      dev = new ttt_device ();
      meas_led->value (1);
      o->copy_label (gettext ("trennen"));
      
      btn_start->activate ();      

      double mmax = dev->get_max_torque ();
      vo_mmax->value (mmax);
      // Start and Stop threshold hard coded 2% and 1%
      peakd.set_thresholds (0.02 * mmax, 0.01 * mmax, vi_peak1_thres->value () / 100.0);
    }
    catch (std::runtime_error &e)
    {
      fl_alert (e.what ());
    }
  }
else
  {
     delete dev;
     dev = 0;
     meas_led->value (0);
     o->copy_label (gettext ("verbinden"));
 
     btn_start->deactivate ();      
     btn_stop->deactivate (); 
  }

mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {850 35 105 40} box GTK_UP_BOX
      }
    }
  }
  code {try
  {
    mainwin->show(argc, argv);
    return Fl::run();
   }
catch (std::runtime_error &e)
   {
    fl_alert (e.what ());
    return -1;
   }} {}
}

Function {run_cb(void*)} {open return_type void
} {
  code {static int cnt = 0;
vector<double> tmp = dev->poll_measurement ();

if (tmp.size () > 0)
  vo_value->value (tmp.back());

for (unsigned int k=0; k<tmp.size (); ++k)
  {
    cplot->add_point (cnt++/900.0, tmp[k]);
    bool r = peakd.update (tmp[k]);
    if (r)
      {
        printf ("new peakset\\n");
        cplot->update_limits ();
        cplot->redraw ();
      }

  }

//values.reserve (values.size () + tmp.size ());
//values.insert (values.end (), tmp.begin (), tmp.end ());
//printf ("size of values = %i\\n", values.size());

if (measuring)
  Fl::repeat_timeout(0.01, run_cb);} {}
}
