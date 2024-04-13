
      static double mp_vector_crop(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const longT
          length = (longT)mp.opcode[3],
          start = (longT)_mp_arg(4),
          sublength = (longT)mp.opcode[5],
          step = (longT)_mp_arg(6);
        if (start<0 || start + step*(sublength-1)>=length)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Value accessor '[]': "
                                      "Out-of-bounds sub-vector request "
                                      "(length: %ld, start: %ld, sub-length: %ld, step: %ld).",
                                      mp.imgin.pixel_type(),length,start,sublength,step);
        ptrs+=start;
        if (step==1) std::memcpy(ptrd,ptrs,sublength*sizeof(double));
        else for (longT k = 0; k<sublength; ++k) { *(ptrd++) = *ptrs; ptrs+=step; }
        return cimg::type<double>::nan();