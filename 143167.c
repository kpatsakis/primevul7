
      static double mp_complex_div_vv(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1,
          r1 = *(ptr1++), i1 = *ptr1,
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        const double denom = r2*r2 + i2*i2;
        *(ptrd++) = (r1*r2 + i1*i2)/denom;
        *ptrd = (r2*i1 - r1*i2)/denom;
        return cimg::type<double>::nan();