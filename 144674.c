
      static double mp_rot3d(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const float x = (float)_mp_arg(2), y = (float)_mp_arg(3), z = (float)_mp_arg(4), theta = (float)_mp_arg(5);
        CImg<doubleT>(ptrd,3,3,1,1,true) = CImg<doubleT>::rotation_matrix(x,y,z,theta);
        return cimg::type<double>::nan();