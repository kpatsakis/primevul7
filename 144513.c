
      static double mp_complex_pow_vv(_cimg_math_parser& mp) {
        const double *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(ptr1[0],ptr1[1],ptr2[0],ptr2[1],ptrd);
        return cimg::type<double>::nan();