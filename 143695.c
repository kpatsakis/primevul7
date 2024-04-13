
      static double mp_vector_copy(_cimg_math_parser& mp) {
        std::memcpy(&_mp_arg(1) + 1,&_mp_arg(2) + 1,sizeof(double)*mp.opcode[3]);
        return cimg::type<double>::nan();