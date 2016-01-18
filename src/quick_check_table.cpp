#include "quick_check_table.h"

void quick_check_table::DrawHeader(const char *s, int X, int Y, int W, int H)
{
  fl_push_clip(X,Y,W,H);
  fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
  fl_color(FL_BLACK);
  fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
  fl_pop_clip();
}

void quick_check_table::DrawData(const char *s, int X, int Y, int W, int H, Fl_Color bgcolor)
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
void quick_check_table::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H)
{
  static char s[40];
  switch ( context )
    {
    case CONTEXT_STARTPAGE:                   // before page is drawn..
      fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
      return;
    case CONTEXT_COL_HEADER:                  // Draw column headers
      snprintf (s, 40, "%.2f", nominal_value);
      DrawHeader(s,X,Y,W,H);
      return;
    case CONTEXT_ROW_HEADER:                  // Draw row headers
      sprintf(s,"%d:", ROW + 1);
      DrawHeader(s,X,Y,W,H);
      return;
    case CONTEXT_CELL:                        // Draw data in cells
      if (ROW < peak_values.size ())
        {
          snprintf (s, 40, "%.2f", peak_values[ROW]);
          DrawData(s,X,Y,W,H,FL_WHITE);
        }
      return;
    default:
      return;
    }
}

quick_check_table::quick_check_table(int X, int Y, int W, int H, const char *L)
  : Fl_Table(X,Y,W,H,L)
{
  // Rows
  rows(0);             // how many rows
  row_header(1);              // enable row headers (along left)
  row_height_all(30);         // default height of rows
  row_header_width (70);
  row_resize(0);              // disable row resizing

  // Cols
  cols(1);              // how many columns
  col_header(1);        // enable column headers (along top)
  col_width_all (80);
  //col_resize(1);        // enable column resizing

}
quick_check_table::~quick_check_table()
{

}
