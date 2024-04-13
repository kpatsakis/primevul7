
      static double mp_complex_pow_ss(_cimg_math_parser& mp) {
        const double val1 = _mp_arg(2), val2 = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(val1,0,val2,0,ptrd);
        return cimg::type<double>::nan();