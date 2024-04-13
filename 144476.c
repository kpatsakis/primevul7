
      static double mp_gauss(_cimg_math_parser& mp) {
        const double x = _mp_arg(2), s = _mp_arg(3);
        return std::exp(-x*x/(2*s*s))/(_mp_arg(4)?std::sqrt(2*s*s*cimg::PI):1);