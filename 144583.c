
      static double mp_rot2d(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const float
          theta = (float)_mp_arg(2)*cimg::PI/180,
          ca = std::cos(theta),
          sa = std::sin(theta);
        *(ptrd++) = ca;
        *(ptrd++) = -sa;
        *(ptrd++) = sa;
        *ptrd = ca;
        return cimg::type<double>::nan();