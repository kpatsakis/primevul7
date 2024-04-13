
      static double mp_kth(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<doubleT> vals(i_end - 4);
        double *p = vals.data();
        for (unsigned int i = 4; i<i_end; ++i) *(p++) = _mp_arg(i);
        int ind = (int)cimg::round(_mp_arg(3));
        if (ind<0) ind+=vals.width() + 1;
        ind = std::max(1,std::min(vals.width(),ind));
        return vals.kth_smallest(ind - 1);