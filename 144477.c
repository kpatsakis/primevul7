
      static double mp_jxyzc(_cimg_math_parser& mp) {
        const unsigned int
          interpolation = (unsigned int)_mp_arg(6),
          boundary_conditions = (unsigned int)_mp_arg(7);
        const CImg<T> &img = mp.imgin;
        const double
          ox = mp.mem[_cimg_mp_slot_x], oy = mp.mem[_cimg_mp_slot_y],
          oz = mp.mem[_cimg_mp_slot_z], oc = mp.mem[_cimg_mp_slot_c],
          x = ox + _mp_arg(2), y = oy + _mp_arg(3),
          z = oz + _mp_arg(4), c = oc + _mp_arg(5);
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const float
              w2 = 2.f*img.width(), h2 = 2.f*img.height(), d2 = 2.f*img.depth(), s2 = 2.f*img.spectrum(),
              mx = cimg::mod((float)x,w2), my = cimg::mod((float)y,h2),
              mz = cimg::mod((float)z,d2), mc = cimg::mod((float)c,s2);
            return (double)img._cubic_atXYZ(mx<img.width()?mx:w2 - mx - 1,
                                            my<img.height()?my:h2 - my - 1,
                                            mz<img.depth()?mz:d2 - mz - 1,
                                            (int)(mc<img.spectrum()?mc:s2 - mc - 1));
          }
          case 2 : // Periodic
            return (double)img._cubic_atXYZ_p((float)x,(float)y,(float)z,
                                              (int)cimg::mod(c,(double)img._spectrum));
          case 1 : // Neumann
            return (double)img._cubic_atXYZ((float)x,(float)y,(float)z,
                                            (int)(c<0?0:c>=img._spectrum?img._spectrum - 1:c));
          default : // Dirichlet
            if (c<0 || c>=img._spectrum) return (T)0;
            return (double)img.cubic_atXYZ((float)x,(float)y,(float)z,(int)c,(T)0);
          }
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const float
              w2 = 2.f*img.width(), h2 = 2.f*img.height(), d2 = 2.f*img.depth(), s2 = 2.f*img.spectrum(),
              mx = cimg::mod((float)x,w2), my = cimg::mod((float)y,h2),
              mz = cimg::mod((float)z,d2), mc = cimg::mod((float)c,s2);
            return (double)img._linear_atXYZ(mx<img.width()?mx:w2 - mx - 1,
                                             my<img.height()?my:h2 - my - 1,
                                             mz<img.depth()?mz:d2 - mz - 1,
                                             (int)(mc<img.spectrum()?mc:s2 - mc - 1));
          }
          case 2 : // Periodic
            return (double)img._linear_atXYZ_p((float)x,(float)y,(float)z,
                                               (int)cimg::mod(c,(double)img._spectrum));
          case 1 : // Neumann
            return (double)img._linear_atXYZ((float)x,(float)y,(float)z,
                                             (int)(c<0?0:c>=img._spectrum?img._spectrum - 1:c));
          default : // Dirichlet
            if (c<0 || c>=img._spectrum) return (T)0;
            return (double)img.linear_atXYZ((float)x,(float)y,(float)z,(int)c,(T)0);
          }
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mx = cimg::mod((int)x,w2), my = cimg::mod((int)y,h2),
              mz = cimg::mod((int)z,d2), mc = cimg::mod((int)c,s2);
            return (double)img(mx<img.width()?mx:w2 - mx - 1,
                               my<img.height()?my:h2 - my - 1,
                               mz<img.depth()?mz:d2 - mz - 1,
                               mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img((int)cimg::mod(x,(double)img._width),
                               (int)cimg::mod(y,(double)img._height),
                               (int)cimg::mod(z,(double)img._depth),
                               (int)cimg::mod(c,(double)img._spectrum));
          case 1 : // Neumann
            return (double)img._atXYZC((int)x,(int)y,(int)z,(int)c);
          default : // Dirichlet
            return (double)img.atXYZC((int)x,(int)y,(int)z,(int)c,(T)0);
          }
        }