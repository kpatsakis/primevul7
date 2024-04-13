
      static double mp_complex_pow_vs(_cimg_math_parser& mp) {
        const double *ptr1 = &_mp_arg(2) + 1, val2 = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(ptr1[0],ptr1[1],val2,0,ptrd);
        return cimg::type<double>::nan();