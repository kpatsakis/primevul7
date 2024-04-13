
      static double mp_complex_conj(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        *(ptrd++) = *(ptrs++);
        *ptrd = -*(ptrs);
        return cimg::type<double>::nan();