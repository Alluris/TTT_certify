##

function ret = lazy_index (v, i, na = 0)
  out_of = i < 1 | i > numel (v);
  i (out_of) = 1;
  ret = v(i);
  ret (out_of) = na;
endfunction

%!test
%! v = 1:9;
%! i = [-3 0 1 6 10 2];
%! assert (lazy_index (v, i), [0 0 1 6 0 2]);
