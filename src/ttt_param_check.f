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

decl {\#define FS 900.0} {private local
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
    label {TTT_Parameter-Check V1.01.002 Alluris GmbH & Co. KG, Basler Str. 65 , 79100 Freiburg, software@alluris.de} open
    xywh {2554 271 1045 710} type Double color 40 resizable size_range {894 544 0 0} visible
  } {
    Fl_Box cplot {
      xywh {5 3 765 701}
      class cairo_plot
    }
    Fl_Group {} {
      label Messung open
      xywh {775 138 265 233} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Button btn_start {
        label Start
        callback {if (dev)
  {
    dev->start ();
    measuring = 1;
    o->deactivate ();
    btn_stop->activate ();
    values.clear ();
    Fl::add_timeout(0.01, run_cb);
  }}
        xywh {785 179 85 35} box GLEAM_UP_BOX deactivate
      }
      Fl_Button btn_stop {
        label Stop
        callback {if (dev)
  {
    dev->stop ();
    measuring = false;
    o->deactivate ();
    btn_start->activate ();
  }}
        xywh {785 224 85 35} box GLEAM_UP_BOX deactivate
      }
      Fl_Value_Output vo_value {
        label {Messwert [Nm]}
        xywh {880 193 150 65} align 133 step 0.1 textsize 49
      }
      Fl_Value_Slider vi_peak1_thres {
        label {Peakdetektion [%]}
        callback {// keep current view
update_cplot(true);}
        tooltip {typisch 80% .. 90%} xywh {785 285 244 30} type {Horz Knob} align 1 minimum 30 maximum 99 step 1 value 90 textsize 14
      }
      Fl_Choice choice_direction {
        label Funktionsrichtung open selected
        xywh {930 331 100 25} down_box BORDER_BOX align 132
      } {
        MenuItem {} {
          label rechts
          xywh {0 0 100 20}
        }
        MenuItem {} {
          label links
          xywh {0 0 100 20}
        }
      }
    }
    Fl_Group {} {
      label Ergebnis open
      xywh {775 374 265 155} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Output vo_peak1 {
        label {Peak 1 [Nm]}
        xywh {965 404 55 30} color 2 step 0.1
      }
      Fl_Value_Output vo_min1 {
        label {Minimum nach Peak 1 [Nm]}
        xywh {965 444 55 30} color 4 align 132 step 0.1 textcolor 255
      }
      Fl_Value_Output vo_peak2 {
        label {Peak 2 [Nm]}
        xywh {965 484 55 30} color 1 step 0.1
      }
    }
    Fl_Group {} {
      label {Messgerät} open
      xywh {775 5 265 130} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Output vo_mmax {
        label {Nominalwert [Nm]}
        xywh {955 90 60 30} step 0.1 value 50
      }
      Fl_Check_Button meas_led {
        xywh {865 35 35 40} type Normal down_box ROUND_DOWN_BOX selection_color 63 labelsize 25 when 0 deactivate
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
    btn_stop->do_callback ();
    delete dev;
    dev = 0;
    meas_led->value (0);
    o->copy_label (gettext ("verbinden"));

    btn_start->deactivate ();
    btn_stop->deactivate ();
  }

mainwin->cursor (FL_CURSOR_DEFAULT);}
        xywh {910 35 105 40} box GLEAM_UP_BOX
      }
    }
    Fl_Group {} {
      label Beispiele open
      xywh {775 533 264 172} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Button btn_csv1 {
        label {\#1}
        callback {choice_direction->value(0);
string fn = "./examples/Stahlwille_MANOSKOP_730_4_610315061_Hand.csv";
load_example (fn);}
        xywh {970 632 30 30} box GLEAM_UP_BOX
      }
      Fl_Button btn_csv2 {
        label {\#2}
        callback {choice_direction->value(0);
string fn = "./examples/Stahlwille_MANOSKOP_730_4_610315061_Hand_2.csv";
load_example (fn);}
        xywh {970 662 30 30} box GLEAM_UP_BOX
      }
      Fl_Button btn_csv3 {
        label {\#1}
        callback {choice_direction->value(0);
string fn = "./examples/Garant_65_6050_6_SN15-492265_Hand.csv";
load_example (fn);}
        xywh {970 577 30 30} box GLEAM_UP_BOX align 128
      }
      Fl_Box {} {
        label {Garant 65 6050 6}
        image {examples/Garant_65_6050_6_SN15-492265.png} xywh {782 564 170 65}
      }
      Fl_Box {} {
        label {MANOSKOP 730/4}
        image {examples/Stahlwille_MANOSKOP_730_4_610315061.png} xywh {782 630 170 65}
      }
    }
  }
  code {//run
try
  {
    mainwin->show();
    // load example on startup
    // btn_csv->do_callback ();

    cplot->set_xlabel ("t [s]");
    cplot->set_ylabel ("M [Nm]");
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
  code {static vector<double> in_buf;

if (measuring)
  {
    vector<double> tmp = dev->poll_measurement ();

    //append new values
    in_buf.reserve (in_buf.size () + tmp.size ());
    in_buf.insert (in_buf.end (), tmp.begin (), tmp.end ());
    //printf ("size of in_buf = %i\\n", in_buf.size());

    if (tmp.size () > 0)
      vo_value->value (tmp.back());

    // feed into peakdetection
    for (unsigned int k=0; k<tmp.size (); ++k)
      {
        bool r = peakd.update (rot () * tmp[k]);
        if (r)
          {
            printf ("new peakset in peakd\\n");
            peakd.print_stats ();
            peakset last = peakd.get_last_peakset ();

            //copy int0 values, 0.5s before/after start/stop
            int start = last.start_x - 0.2 * FS;
            if (start < 0)
              start = 0;
            int stop = last.stop_x + 0.2 * FS;
            if (stop >= int(in_buf.size ()))
              stop = in_buf.size () - 1;

            vector<double>::iterator it = in_buf.begin();
            values.assign (it+start, it+stop);

            update_cplot ();

            in_buf.clear();
            peakd.clear ();
            break;
          }
      }
    Fl::repeat_timeout(0.01, run_cb);
  }} {}
}

Function {update_cplot(bool keep_view = 0)} {open return_type void
} {
  code {//printf ("size of values = %i\\n", values.size());

ttt_peak_detector tmp_peakd;
// Start and Stop threshold hard coded 2% and 1%
tmp_peakd.set_thresholds (0.02 * vo_mmax->value(), 0.01 * vo_mmax->value(), vi_peak1_thres->value () / 100.0);

// store view
double x0;
double x1;
double y0;
double y1;
if (keep_view)
  {
    cplot->get_xlim (x0, x1);
    cplot->get_ylim (y0, y1);
  }

// feed values in peak detector
for (unsigned int k=0; k < values.size (); ++k)
  {
    bool r = tmp_peakd.update (rot () * values[k]);
    if (r)
      {
        //printf ("new peakset\\n");
        tmp_peakd.print_stats ();

        peakset last = tmp_peakd.get_last_peakset ();

        // 0.2s before start
        int start = last.start_x - 0.2 * FS;
        if (start < 0)
          start = 0;
        // 0.2s after stop
        int stop = last.stop_x + 0.2 * FS;
        if (stop >= int(values.size ()))
          stop = values.size () - 1;

        // feed into cplot
        cplot->clear ();
        for (int j=0; j <= (stop - start); ++j)
          cplot->add_point (j/FS, rot ()* values[start + j]);

        if (keep_view)
          {
            cplot->set_xlim (x0, x1);
            cplot->set_ylim (y0, y1);
          }
        else
          cplot->update_limits ();

        // place marker
        if (last.peak1_x > 0)
          {
            cplot->add_marker ((last.peak1_x - start)/FS, last.peak1_y, 15, 0, 1, 0);
            vo_peak1->value (last.peak1_y);
          }
        else
          vo_peak1->value (0);

        if (last.min_after_peak1_x > 0)
          {
            cplot->add_marker ((last.min_after_peak1_x - start)/FS, last.min_after_peak1_y, 15, 0, 0, 1);
            vo_min1->value (last.min_after_peak1_y);
          }
        else
          vo_min1->value (0);

        if (last.peak2_x > 0)
          {
            cplot->add_marker ((last.peak2_x - start)/FS, last.peak2_y, 15, 1, 0, 0);
            vo_peak2->value (last.peak2_y);
          }
        else
          vo_peak2->value (0);

        cplot->redraw ();
        break;
      }
  }} {}
}

Function {load_example(string fn)} {open
} {
  code {double value;
int cnt = 0;
ifstream in (fn.c_str ());
if (in.is_open())
  {
    values.clear ();
    while (! in.fail ())
      {
        in >> value;
        values.push_back (value);
        cnt++;
      }

    if (in.fail () && ! in.eof ())
      cerr << "Couldn't read double in line " << cnt << endl;
    //cout << "read " << cnt << " values..." << endl;

    in.close();
    update_cplot();
  }
else
  {
    fl_alert (gettext ("Die Datei '%s' konnte nicht gefunden werden"), fn.c_str());
    cerr << "Unable to open file '" << fn << "'" << endl;
  }} {}
}

Function {rot()} {open return_type double
} {
  code {int s = choice_direction->value ();
assert (s >= 0);
assert (s <= 1);
return 1 - (s * 2);} {}
}
