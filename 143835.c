
      static double mp_self_modulo(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = cimg::mod(val,_mp_arg(2));