
      static double mp_ellipse(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.listin.width());
        CImg<T> &img = ind==~0U?mp.imgout:mp.listout[ind];
        CImg<T> color(img._spectrum,1,1,1,0);
        bool is_invalid_arguments = false, is_outlined = false;
        float r1 = 0, r2 = 0, angle = 0, opacity = 1;
        unsigned int i = 4, pattern = ~0U;
        int x0 = 0, y0 = 0;
        if (i>=i_end) is_invalid_arguments = true;
        else {
          x0 = (int)cimg::round(_mp_arg(i++));
          if (i>=i_end) is_invalid_arguments = true;
          else {
            y0 = (int)cimg::round(_mp_arg(i++));
            if (i>=i_end) is_invalid_arguments = true;
            else {
              r1 = (float)_mp_arg(i++);
              if (i>=i_end) r2 = r1;
              else {
                r2 = (float)_mp_arg(i++);
                if (i<i_end) {
                  angle = (float)_mp_arg(i++);
                  if (i<i_end) {
                    opacity = (float)_mp_arg(i++);
                    if (r1<0 && r2<0) {
                      pattern = (unsigned int)_mp_arg(i++);
                      is_outlined = true;
                      r1 = -r1; r2 = -r2;
                    }
                    if (i<i_end) {
                      cimg_forX(color,k) if (i<i_end) color[k] = (T)_mp_arg(i++);
                      else { color.resize(k,1,1,1,-1); break; }
                      color.resize(img._spectrum,1,1,1,0,2);
                    }
                  }
                }
              }
            }
          }
        }
        if (!is_invalid_arguments) {
          if (is_outlined) img.draw_ellipse(x0,y0,r1,r2,angle,color._data,opacity,pattern);
          else img.draw_ellipse(x0,y0,r1,r2,angle,color._data,opacity);
        } else {
          CImg<doubleT> args(i_end - 4);
          cimg_forX(args,k) args[k] = _mp_arg(4 + k);
          if (ind==~0U)
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'ellipse()': "
                                        "Invalid arguments '%s'. ",
                                        mp.imgin.pixel_type(),args.value_string()._data);
          else
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'ellipse()': "
                                        "Invalid arguments '#%u%s%s'. ",
                                        mp.imgin.pixel_type(),ind,args._width?",":"",args.value_string()._data);
        }
        return cimg::type<double>::nan();