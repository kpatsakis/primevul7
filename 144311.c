
      static double mp_matrix_eig(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptr1 = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        CImg<doubleT> val, vec;
        CImg<doubleT>(ptr1,k,k,1,1,true).symmetric_eigen(val,vec);
        CImg<doubleT>(ptrd,1,k,1,1,true) = val;
        CImg<doubleT>(ptrd + k,k,k,1,1,true) = vec.get_transpose();
        return cimg::type<double>::nan();