## 02.02.2016 Andreas Weber
## in einer Schleife immer den letzen Peak anzeigen

pkg load signal
FS = 900;

last_fn = sort (glob ("*.log"), "ascend"){end};
printf ("Opening file \"%s\"...\n", last_fn);
fid = fopen (last_fn, "r");

disp (fgetl (fid));
disp (fgetl (fid));

live_plot_len = 6 * FS;
steady_plot_len = 3 * FS;
v = zeros (live_plot_len, 1);
t = linspace (-live_plot_len/FS, 0, live_plot_len);
subplot (2, 1, 1);
p = plot (t, v);
set (gca, "ylim", [0 14])
grid on
xlabel ("t [s]");
ylabel ("M [Nm]");

do
  [tmp, cnt] = fscanf (fid, "%f");
  tmp = tmp(1:2:end);
  if (numel (tmp) >= live_plot_len);
    v = tmp (end-live_plot_len+1:end);
  else
    v = [v(numel(tmp)+1:end); tmp];
  endif
  set (p, "ydata", v);

  ## Strategie:
  ## Suche die späteste Stelle, an der der Wert unter 5%
  ## des Maximums fällt
  mv = max (v);
  ## Nur maximas, die über 1.5N liegen akzeptieren
  if (mv > 1.5)
    ind_n = find( diff (v < (0.05 * mv)), 1, "last");
  else
    ind_n = [];
  endif

  pre_i = round (0.8 * steady_plot_len);
  if (! isempty (ind_n) && ind_n > pre_i)
    ## 80% vor ind und 20% nach ind
    r = ind_n-pre_i+1:ind_n+steady_plot_len-pre_i;
    v_steady = lazy_index (v, r);
    subplot (2, 1, 2);
    plot_peak (v_steady, FS, "t [s]", "M [Nm]", [0 14]);
  endif

  drawnow ("expose");
  #max(v(end-plot_len+1:end))
  fflush (stdout);
  fclear (fid);
  sleep (0.05)
until (kbhit (1) || !ishandle(p))

printf ("Closing file \"%s\"...\n", last_fn);
fclose (fid);
