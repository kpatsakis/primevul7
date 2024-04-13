
      static double mp_vector_resize(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int p1 = (unsigned int)mp.opcode[2], p2 = (unsigned int)mp.opcode[4];
        const int
          interpolation = (int)_mp_arg(5),
          boundary_conditions = (int)_mp_arg(6);
        if (p2) { // Resize vector
          const double *const ptrs = &_mp_arg(3) + 1;
          CImg<doubleT>(ptrd,p1,1,1,1,true) = CImg<doubleT>(ptrs,p2,1,1,1,true).
            get_resize(p1,1,1,1,interpolation,boundary_conditions);
        } else { // Resize scalar
          const double value = _mp_arg(3);
          CImg<doubleT>(ptrd,p1,1,1,1,true) = CImg<doubleT>(1,1,1,1,value).resize(p1,1,1,1,interpolation,
                                                                                  boundary_conditions);
        }
        return cimg::type<double>::nan();