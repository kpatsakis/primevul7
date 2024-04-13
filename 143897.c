
      static double mp_self_bitwise_left_shift(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val<<(unsigned int)_mp_arg(2));