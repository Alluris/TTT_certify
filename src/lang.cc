#include <string.h>
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
  Deutsch:     "de_DE.utf8"
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
      printf ("DEBUG: putenv(%s)\n", lang_buf);

    std::string tmp;
    if (! strncmp (l, "de", 2))
      tmp = "german";
    else if (! strncmp (l, "en", 2))
      tmp = "english";
    else if (! strncmp (l, "es", 2))
      tmp = "spanish";
    else if (! strncmp (l, "fr", 2))
      tmp = "french";
    else if (! strncmp (l, "it", 2))
      tmp = "italian";
    else
    fprintf (stderr, "ERROR: couldn't map '%s'\n", l);

    if (! tmp.empty ())
    {
      printf ("DEBUG: init_lang map '%s' to '%s'\n", l, tmp.c_str ());
      setlocale (LC_ALL, tmp.c_str ());
    }
    }
  else
   setlocale (LC_ALL, "");
#else
  setlocale (LC_ALL, "");
#endif

  bindtextdomain("ttt","./po");
  textdomain ("ttt");

  printf ("DEBUG: Anbauteile = %s\n", gettext ("Anbauteile"));
  printf ("DEBUG: pi = %g\n", 3.14159265359);
}
