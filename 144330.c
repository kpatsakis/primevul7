
      static double mp_cross(_cimg_math_parser& mp) {
        CImg<doubleT>
          vout(&_mp_arg(1) + 1,1,3,1,1,true),
          v1(&_mp_arg(2) + 1,1,3,1,1,true),
          v2(&_mp_arg(3) + 1,1,3,1,1,true);
        (vout = v1).cross(v2);
        return cimg::type<double>::nan();