#include <FL/Fl.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <vector>

#ifndef _QUICK_CHECK_TABLE_
#define _QUICK_CHECK_TABLE_

using namespace std;

class quick_check_table : public Fl_Table
{
private:
  double nominal_value;
  vector <double> peak_values;

  void DrawHeader(const char *s, int X, int Y, int W, int H);
  void DrawData(const char *s, int X, int Y, int W, int H, Fl_Color bgcolor);
  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0);

public:

  quick_check_table(int X, int Y, int W, int H, const char *L=0);
  ~quick_check_table();

  void clear ()
  {
    peak_values.clear ();
    rows (5);
  }

  void set_nominal_value (double nom)
  {
    nominal_value = nom;
  }

  void add_measurement (double v)
  {
    peak_values.push_back (v);
    rows (peak_values.size ());
    redraw ();
    cout << "add_measurement v=" << v << " size=" << peak_values.size () << endl;

  }

};

#endif
