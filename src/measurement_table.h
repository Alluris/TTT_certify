#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include "sqlite_interface.h"

#ifndef _MEASUREMENT_TABLE_
#define _MEASUREMENT_TABLE_

class measurement_table : public Fl_Table
{
private:
  vector <double> nominal_values;
  vector <double> peak_values;
  vector <Fl_Color> colors;
  bool next_measurement_replaces_last;

  void DrawHeader(const char *s, int X, int Y, int W, int H);
  void DrawData(const char *s, int X, int Y, int W, int H, Fl_Color bgcolor);
  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0);

public:

  measurement_table(int X, int Y, int W, int H, const char *L=0);
  ~measurement_table();

  void clear ()
  {
    nominal_values.clear ();
    peak_values.clear ();
    colors.clear ();
  }

  void add_nominal_value (double nom)
  {
    nominal_values.push_back (nom);
  }

  void add_measurement (double v, bool overwrite = false, Fl_Color c = FL_WHITE)
  {
    //cout << "add_measurement v=" << v << " overwrite=" << overwrite << " c=" << c << endl;
    if (next_measurement_replaces_last)
      {
        peak_values.pop_back ();
        colors.pop_back ();
      }

    peak_values.push_back (v);
    colors.push_back (c);
    next_measurement_replaces_last = overwrite;
    redraw ();
  }



};

#endif
