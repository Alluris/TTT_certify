function ret = has_no_scale (to)
  ret = (strcmp (to.type, "II") && strfind ("BCEF", toupper (to.class)));
endfunction

%!assert (has_no_scale (struct ("type", "I", "class", "A")), false)
%!assert (has_no_scale (struct ("type", "I", "class", "B")), false)
%!assert (has_no_scale (struct ("type", "I", "class", "C")), false)
%!assert (has_no_scale (struct ("type", "I", "class", "D")), false)
%!assert (has_no_scale (struct ("type", "I", "class", "E")), false)
%!assert (has_no_scale (struct ("type", "II", "class", "A")), false)
%!assert (has_no_scale (struct ("type", "II", "class", "B")), true)
%!assert (has_no_scale (struct ("type", "II", "class", "C")), true)
%!assert (has_no_scale (struct ("type", "II", "class", "D")), false)
%!assert (has_no_scale (struct ("type", "II", "class", "E")), true)
%!assert (has_no_scale (struct ("type", "II", "class", "F")), true)
%!assert (has_no_scale (struct ("type", "II", "class", "G")), false)
