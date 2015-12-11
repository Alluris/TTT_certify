function ret = is_screwdriver (to)
  tc = strcat (to.type, to.class);
  ret =    strcmp (toupper(tc), "ID")...
        || strcmp (toupper(tc), "IE")...
        || strcmp (toupper(tc), "IID")...
        || strcmp (toupper(tc), "IIE")...
        || strcmp (toupper(tc), "IIF");
endfunction

%!assert (is_screwdriver (struct ("type", "I", "class", "A")), false)
%!assert (is_screwdriver (struct ("type", "I", "class", "B")), false)
%!assert (is_screwdriver (struct ("type", "I", "class", "C")), false)
%!assert (is_screwdriver (struct ("type", "I", "class", "D")), true)
%!assert (is_screwdriver (struct ("type", "I", "class", "E")), true)
%!assert (is_screwdriver (struct ("type", "II", "class", "A")), false)
%!assert (is_screwdriver (struct ("type", "II", "class", "B")), false)
%!assert (is_screwdriver (struct ("type", "II", "class", "C")), false)
%!assert (is_screwdriver (struct ("type", "II", "class", "D")), true)
%!assert (is_screwdriver (struct ("type", "II", "class", "E")), true)
%!assert (is_screwdriver (struct ("type", "II", "class", "F")), true)
%!assert (is_screwdriver (struct ("type", "II", "class", "G")), false)
