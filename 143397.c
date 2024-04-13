
      static double mp_srand(_cimg_math_parser& mp) {
        mp.rng = (ulongT)_mp_arg(2);
        return cimg::type<double>::nan();