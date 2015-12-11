## get rise_time from 80% to first_peak
## from click data

function ret = get_rise_time (d)

  trigger = 0.5;
  ind = find (d < cummax (d)*trigger, 1);
  d = d(1:ind);

  [m, i1] = max (d);
  i2 = find (d >= 0.8 * m, 1);

  ret = (i1-i2)/900;
endfunction

%!test
%! d = click_peak (2.5, 1.23, 1.56, 900);
%! t = get_rise_time (d);
%! assert (t, 0.6, 0.01)
