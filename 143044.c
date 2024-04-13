
      static double* _mp_memcopy_double(_cimg_math_parser& mp, const unsigned int ind, const ulongT *const p_ref,
                                        const longT siz, const long inc) {
        const longT
          off = *p_ref?p_ref[1] + (longT)mp.mem[(longT)p_ref[2]] + 1:ind,
          eoff = off + (siz - 1)*inc;
        if (off<0 || eoff>=mp.mem.width())
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'copy()': "
                                      "Out-of-bounds variable pointer "
                                      "(length: %ld, increment: %ld, offset start: %ld, "
                                      "offset end: %ld, offset max: %u).",
                                      mp.imgin.pixel_type(),siz,inc,off,eoff,mp.mem._width - 1);
        return &mp.mem[off];