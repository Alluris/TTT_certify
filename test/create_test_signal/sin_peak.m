# len in seconds
function y = sin_peak (len, amp, FS = 900)
  x = linspace (0, 2 * pi, len * FS)';
  y = amp/2 * (1 - cos (x));

  # Anstiegszeit ausgeben
  # von 80% bis max
  #i = find (y >= 0.8 * amp, 1);
  #t_rise = len/2 - i / FS;
  #printf ("t_rise = %.3fs\n", t_rise);

  y = [y, zeros(rows(y), 1)];
endfunction

%!demo
%! y = sin_peak (5, 1);
%! plot (y)
