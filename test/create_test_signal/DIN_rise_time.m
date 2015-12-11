function [mi, ma] = DIN_rise_time (torque, to)
  if (is_screwdriver (to))
    mi = 0.5;
    ma = 1.0;
  else
    ma = 0;
    if (torque < 10)
      mi = 0.5;
    elseif (torque < 100)
      mi = 1.0;
    elseif (torque < 1000)
      mi = 1.5;
    else
      mi = 2;
    endif
  endif
endfunction


%!test
%! [mi, ma] = DIN_rise_time (5, struct ("type", "I", "class", "A"));
%! assert (mi, 0.5);
%! assert (ma, 0);

%!test
%! [mi, ma] = DIN_rise_time (15, struct ("type", "I", "class", "B"));
%! assert (mi, 1.0);
%! assert (ma, 0);

%!test
%! [mi, ma] = DIN_rise_time (500, struct ("type", "I", "class", "C"));
%! assert (mi, 1.5);
%! assert (ma, 0);

%!test
%! [mi, ma] = DIN_rise_time (5000, struct ("type", "I", "class", "C"));
%! assert (mi, 2.0);
%! assert (ma, 0);

%!test
%! [mi, ma] = DIN_rise_time (5, struct ("type", "I", "class", "D"));
%! assert (mi, 0.5);
%! assert (ma, 1);
