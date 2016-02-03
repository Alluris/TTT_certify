## Copyright (C) 2016 Andreas Weber
##
## This file is part of ttt_certify.
##
## ttt_certify is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## ttt_certify is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with ttt_certify; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {} {@var{ret} =} plot_peak (@var{d}, @var{FS}, @var{xl}, @var{yl}, @var{ylim})
## Plot a click peak.
##
## @end deftypefn

function plot_peak (d, FS, xl, yl, ylim)
  if (nargin != 5)
    print_usage ();
  endif

  t = ((-(numel (d)-1)):0)/FS;
  cumd = cummax (d);
  ## Abfall unter trigger * cummax UND cummax größer 20% vom maximum
  trigger = 0.5;
  ind = find ((d < cumd * trigger) & (cumd > 0.2 * max (d)), 1);

  [m, i2] = max (d(1:ind));
  i1 = find (d(1:ind) >= 0.8 * m, 1);
  rise_time = (i2-i1)/FS;

  [gmax, gmax_ind] = max (d);
  peak_min = min (d(ind:gmax_ind));

  if (isempty (m))
    m = NA;
  endif

  if (isempty (peak_min))
    peak_min = NA;
  endif

  if (isempty (rise_time))
    rise_time = NA;
  endif

  printf ("peak1=%5.2fN max=%5.2fN peak_min=%5.2fN risetime=%.2fs\n",
            m,
            gmax,
            peak_min,
            rise_time);

  p = findobj ("-property", "_plot_peak_");
  line1 = findobj ("-property", "_plot_peak_line1_");
  line2 = findobj ("-property", "_plot_peak_line2_");
  #txt = findobj ("-property", "_plot_peak_text_");

  if (isempty (p))
    p = plot (t, d);
    #disp ("new plot...");
    addproperty ("_plot_peak_", p, "any")
    xlabel (xl);
    ylabel (yl);
    set (get (p, "parent"), "ylim", ylim);
    grid on

    line1 = line ([t(1) t(end)], [d(i1) d(i1)], "color", "red")
    addproperty ("_plot_peak_line1_", line1, "any")
    line2 = line ([t(1) t(end)], [d(i2) d(i2)], "color", "red")
    addproperty ("_plot_peak_line2_", line2, "any")

    #txt = text (-2.8, 12, msg);
    #addproperty ("_plot_peak_text_", txt, "any")

  else
    #disp ("reusing old plot...");
    set (p, "ydata", d);
    set (line1, "ydata", [d(i1) d(i1)]);
    set (line2, "ydata", [d(i2) d(i2)]);
    #set (txt, "string", msg);
  endif
endfunction
