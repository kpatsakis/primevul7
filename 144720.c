
      static double mp_cut(_cimg_math_parser& mp) {
        double val = _mp_arg(2), cmin = _mp_arg(3), cmax = _mp_arg(4);
        return val<cmin?cmin:val>cmax?cmax:val;