
      static double mp_self_bitwise_and(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val & (longT)_mp_arg(2));