
      static double mp_self_pow(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = std::pow(val,_mp_arg(2));