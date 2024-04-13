
      static double mp_complex_pow_sv(_cimg_math_parser& mp) {
        const double val1 = _mp_arg(2), *ptr2 = &_mp_arg(3) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(val1,0,ptr2[0],ptr2[1],ptrd);
        return cimg::type<double>::nan();