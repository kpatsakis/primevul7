
      static double mp_complex_div_sv(_cimg_math_parser& mp) {
        const double
          *ptr2 = &_mp_arg(3) + 1,
          r1 = _mp_arg(2),
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        const double denom = r2*r2 + i2*i2;
        *(ptrd++) = r1*r2/denom;
        *ptrd =  -r1*i2/denom;
        return cimg::type<double>::nan();