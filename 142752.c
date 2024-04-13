
      static double mp_complex_log(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1, r = *(ptrs++), i = *(ptrs);
        *(ptrd++) = 0.5*std::log(r*r + i*i);
        *(ptrd++) = std::atan2(i,r);
        return cimg::type<double>::nan();