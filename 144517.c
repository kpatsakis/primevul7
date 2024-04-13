
      static double mp_list_Jxyz(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width()),
          interpolation = (unsigned int)_mp_arg(6),
          boundary_conditions = (unsigned int)_mp_arg(7),
          vsiz = (unsigned int)mp.opcode[8];
        const CImg<T> &img = mp.listin[ind];
        const double
          ox = mp.mem[_cimg_mp_slot_x], oy = mp.mem[_cimg_mp_slot_y], oz = mp.mem[_cimg_mp_slot_z],
          x = ox + _mp_arg(3), y = oy + _mp_arg(4), z = oz + _mp_arg(5);
        const ulongT whd = (ulongT)img._width*img._height*img._depth;
        const T *ptrs;
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const float
              w2 = 2.f*img.width(), h2 = 2.f*img.height(), d2 = 2.f*img.depth(),
              mx = cimg::mod((float)x,w2), my = cimg::mod((float)y,h2), mz = cimg::mod((float)z,d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ_p((float)x,(float)y,(float)z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ((float)x,(float)y,(float)z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img.cubic_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          } break;
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const float
              w2 = 2.f*img.width(), h2 = 2.f*img.height(), d2 = 2.f*img.depth(),
              mx = cimg::mod((float)x,w2), my = cimg::mod((float)y,h2), mz = cimg::mod((float)z,d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._linear_atXYZ(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._linear_atXYZ_p((float)x,(float)y,(float)z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img._linear_atXYZ((float)x,(float)y,(float)z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,vsiz - 1,c) *(ptrd++) = (double)img.linear_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          } break;
        case 0 : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              mx = cimg::mod((int)x,w2), my = cimg::mod((int)y,h2), mz = cimg::mod((int)z,d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 2 : { // Periodic
            const int
              cx = (int)cimg::mod(x,(double)img._width),
              cy = (int)cimg::mod(y,(double)img._height),
              cz = (int)cimg::mod(z,(double)img._depth);
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 1 : { // Neumann
            ptrs = &img._atXYZ((int)x,(int)y,(int)z);
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          default : // Dirichlet
            if (img.containsXYZC((int)x,(int)y,(int)z)) {
              ptrs = &img((int)x,(int)y,(int)z);
              cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
            } else std::memset(ptrd,0,vsiz*sizeof(double));
          }
        }
        return cimg::type<double>::nan();