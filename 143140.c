
      static double mp_complex_mul(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1,
          r1 = *(ptr1++), i1 = *ptr1,
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        *(ptrd++) = r1*r2 - i1*i2;
        *(ptrd++) = r1*i2 + r2*i1;
        return cimg::type<double>::nan();