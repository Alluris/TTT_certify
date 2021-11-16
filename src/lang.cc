#include "lang.h"

/*
  Workaround via C functions for Windows where the binaries are built using
  mingw32 which uses the old msvcrtl.dll (many bugs in locale support).
  Just search for MSVCRT throwing an instance of 'std::runtime_error'
                  what():  locale::facet::_S_create_c_locale name not valid
 */

std::string FloatToStr (double val, const char *unit, int digits)
{
  #define BUFLEN 50
  char buf[BUFLEN];

  int len = 0;
  if (digits >= 0)
    len = snprintf (buf, BUFLEN, "%.*f", digits, val);
  else
    len = snprintf (buf, BUFLEN, "%g", val);
  
  if (unit && *unit)
    snprintf (buf + len, BUFLEN - len, " %s", unit);

  return buf;
}
