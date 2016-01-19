#include <FL/Fl.H>
#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include "sqlite_interface.h"

#ifndef _TEST_PERSON_TABLE_
#define _TEST_PERSON_TABLE_

typedef void(select_cb)(int id);

class test_person_table : public Fl_Table_Row
{
private:

  select_cb* cb;

  void DrawHeader(const char *s, int X, int Y, int W, int H)
  {
    fl_push_clip(X,Y,W,H);
    fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
    fl_color(FL_BLACK);
    fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    fl_pop_clip();
  }

  void DrawData(const char *s, int X, int Y, int W, int H, Fl_Color bgcolor)
  {
    fl_push_clip(X,Y,W,H);
    // Draw cell bg
    fl_color(bgcolor);
    fl_rectf(X,Y,W,H);
    // Draw cell data
    fl_color(FL_GRAY0);
    fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    // Draw box border
    fl_color(color());
    fl_rect(X,Y,W,H);
    fl_pop_clip();
  }

  // Handle drawing table's cells
  //     Fl_Table calls this function to draw each visible cell in the table.
  //     It's up to us to use FLTK's drawing functions to draw the cells the way we want.
  //
  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0)
  {
    static char s[40];
    switch ( context )
      {
      case CONTEXT_STARTPAGE:                   // before page is drawn..
        fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
        return;
      case CONTEXT_COL_HEADER:                  // Draw column headers

        switch (COL)
          {
          case 0:
            snprintf (s, 40, "%s", gettext ("Name"));
            break;
          case 1:
            snprintf (s, 40, "%s", gettext ("Verantwortlicher"));
            break;
          case 2:
            snprintf (s, 40, "%s", gettext ("Messunsicherheit"));
            break;
          }
        DrawHeader(s,X,Y,W,H);
        return;
      case CONTEXT_ROW_HEADER:                  // Draw row headers
        sprintf(s,"%03d:", vtp[ROW].id);
        DrawHeader(s,X,Y,W,H);
        return;
      case CONTEXT_CELL:                        // Draw data in cells
        switch (COL)
          {
          case 0:
            snprintf (s, 40, "%s", vtp[ROW].name.c_str ());
            break;
          case 1:
            snprintf (s, 40, "%s", vtp[ROW].supervisor.c_str ());
            break;
          case 2:
            snprintf (s, 40, "%.1f%%", vtp[ROW].uncertainty * 100);
            break;
          }
        DrawData(s,X,Y,W,H,row_selected (ROW) ? FL_YELLOW : FL_WHITE);
        return;
      default:
        return;
      }
  }

public:

  sqlite3 *db;
  vector<test_person> vtp;

  test_person_table(int X, int Y, int W, int H, const char *L=0) : Fl_Table_Row(X,Y,W,H,L), cb(0)
  {
    // FIXME, retrieve db name from main
    string database_fn = "ttt_certify.db";
    int rc = sqlite3_open(database_fn.c_str (), &db);
    if( rc )
      {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        throw runtime_error ("Can't open sqlite database");
      }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);

    // Rows
    rows(0);             // how many rows
    row_header(1);              // enable row headers (along left)
    row_height_all(20);         // default height of rows
    row_resize(0);              // disable row resizing

    // Cols
    cols(3);              // how many columns
    col_header(1);        // enable column headers (along top)
    col_width (0, 200);   // Name
    col_width (1, 200);   // Verantwortlicher
    col_width (2, 170);   // Messunsicherheit
    col_resize(1);        // enable column resizing

    type (SELECT_SINGLE); // Only single rows can be selected
    Fl_Table::when (FL_WHEN_NEVER);
  }

  ~test_person_table()
  {
    sqlite3_close (db);
  }

  void search_name (string str)
  {
    search (NAME, str);
  }

  void search (enum test_person_search_field field, string str)
  {
    vtp.clear ();
    search_test_persons (db, field, subst_wildcards (str), vtp);
    rows (vtp.size());
  }

  void set_select_cb (select_cb *c)
  {
    cb = c;
  }

  void do_select_cb ()
  {
    int id = get_selected_id ();
    if (cb && id > 0)
      cb (id);
  }

  int get_selected_id ()
  {
    int k;
    for (k = 0; k < rows (); ++k)
      if (row_selected (k))
        return vtp[k].id;

    return 0;
  }

  int handle(int event)
  {
    int ret = Fl_Table_Row::handle(event);
    if (Fl::event_clicks ())
      {
        do_select_cb ();
        return 1;
      }
    return ret;
  }

};

#endif
