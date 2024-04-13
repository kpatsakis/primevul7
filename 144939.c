
      static double mp_continue(_cimg_math_parser& mp) {
        mp.break_type = 2;
        mp.p_code = mp.p_break - 1;
        return cimg::type<double>::nan();