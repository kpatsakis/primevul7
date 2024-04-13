
      static double mp_image_d(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        const CImg<T> &img = ind==~0U?mp.imgout:mp.listout[ind];
        return (double)img.depth();