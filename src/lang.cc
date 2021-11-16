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

/*
  Deutsch:     ""
  Englisch:    "en_US.utf8"
  Spanisch:    "es_ES.utf8"
  Französisch: "fr_FT.utf8"
  Italienisch: "it_IT.utf8"

  gettext uses the environment var "LANG" to select the translation

*/
void init_lang (const char* l)
{
#ifdef _WIN32
  if (l)
    {
      // create string on heap (putenv doesn't copy the string)
      // never free it! (this sounds like a memory leak, I know)
      char *lang_buf = (char *) malloc (50);
      snprintf (lang_buf, 50, "LANG=%s", l);
      putenv(lang_buf);
      std::cout << "DEBUG: putenv(" << lang_buf << ")" << std::endl;
    }
#endif

  setlocale (LC_ALL, "");
  bindtextdomain("ttt","./po");
  textdomain ("ttt");

  printf ("DEBUG: Anbauteile = %s\n", gettext ("Anbauteile"));
}
