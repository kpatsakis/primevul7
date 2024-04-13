
      static double mp_draw(_cimg_math_parser& mp) {
        const int x = (int)_mp_arg(4), y = (int)_mp_arg(5), z = (int)_mp_arg(6), c = (int)_mp_arg(7);
        unsigned int ind = (unsigned int)mp.opcode[3];

        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.listin.width());
        CImg<T> &img = ind==~0U?mp.imgout:mp.listout[ind];
        unsigned int
          dx = (unsigned int)mp.opcode[8],
          dy = (unsigned int)mp.opcode[9],
          dz = (unsigned int)mp.opcode[10],
          dc = (unsigned int)mp.opcode[11];
        dx = dx==~0U?img._width:(unsigned int)_mp_arg(8);
        dy = dy==~0U?img._height:(unsigned int)_mp_arg(9);
        dz = dz==~0U?img._depth:(unsigned int)_mp_arg(10);
        dc = dc==~0U?img._spectrum:(unsigned int)_mp_arg(11);

        const ulongT sizS = mp.opcode[2];
        if (sizS<(ulongT)dx*dy*dz*dc)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Sprite dimension (%lu values) and specified sprite geometry (%u,%u,%u,%u) "
                                      "(%lu values) do not match.",
                                      mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);
        CImg<doubleT> S(&_mp_arg(1) + 1,dx,dy,dz,dc,true);
        const float opacity = (float)_mp_arg(12);

        if (img._data) {
          if (mp.opcode[13]!=~0U) { // Opacity mask specified
            const ulongT sizM = mp.opcode[14];
            if (sizM<(ulongT)dx*dy*dz)
              throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                          "Mask dimension (%lu values) and specified sprite geometry (%u,%u,%u,%u) "
                                          "(%lu values) do not match.",
                                          mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);
            const CImg<doubleT> M(&_mp_arg(13) + 1,dx,dy,dz,(unsigned int)(sizM/(dx*dy*dz)),true);
            img.draw_image(x,y,z,c,S,M,opacity,(float)_mp_arg(15));
          } else img.draw_image(x,y,z,c,S,opacity);
        }
        return cimg::type<double>::nan();