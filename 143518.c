
      static double mp_bitwise_not(_cimg_math_parser& mp) {
        // Limit result to 32bits such that it can be entirely represented as a 'double'.
        return (double)~(unsigned int)_mp_arg(2);