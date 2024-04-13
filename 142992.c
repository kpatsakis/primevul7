
      static double mp_complex_exp(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1, r = *(ptrs++), i = *(ptrs), er = std::exp(r);
        *(ptrd++) = er*std::cos(i);
        *(ptrd++) = er*std::sin(i);
        return cimg::type<double>::nan();