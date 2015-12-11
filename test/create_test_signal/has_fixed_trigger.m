function ret = has_fixed_trigger (to)
  ret = (strcmp (to.type, "II") && strfind ("BE", toupper (to.class)));
endfunction

%!assert (has_fixed_trigger (struct ("type", "I", "class", "A")), false)
%!assert (has_fixed_trigger (struct ("type", "I", "class", "B")), false)
%!assert (has_fixed_trigger (struct ("type", "I", "class", "C")), false)
%!assert (has_fixed_trigger (struct ("type", "I", "class", "D")), false)
%!assert (has_fixed_trigger (struct ("type", "I", "class", "E")), false)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "A")), false)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "B")), true)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "C")), false)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "D")), false)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "E")), true)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "F")), false)
%!assert (has_fixed_trigger (struct ("type", "II", "class", "G")), false)
