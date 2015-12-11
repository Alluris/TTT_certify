function save_sim_input (fn, data)

  fid = fopen ([fn, ".log"], "w");
  fprintf (fid, "%.3f\t%i\n", data');
  fclose (fid);

  img_fn = [fn,".png"];
  if (! exist (img_fn, "file"))
    graphics_toolkit gnuplot;
    t = linspace (0, rows (data)/900, rows (data));
    plot (t, data(:,1), ";torque [Nm];", t, data(:,2), ";confirmation;");
    grid on;
    xlabel ("t [s]");
    ylabel ("M [Nm]");
    print (img_fn);
  endif

endfunction
