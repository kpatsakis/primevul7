
      static double mp_image_stats(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind==~0U) CImg<doubleT>(ptrd,14,1,1,1,true) = mp.imgout.get_stats();
        else {
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
          CImg<doubleT>(ptrd,14,1,1,1,true) = mp.listout[ind].get_stats();
        }
        return cimg::type<double>::nan();