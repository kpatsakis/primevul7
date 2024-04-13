
      static double mp_i(_cimg_math_parser& mp) {
        return (double)mp.imgin.atXYZC((int)mp.mem[_cimg_mp_slot_x],(int)mp.mem[_cimg_mp_slot_y],
                                       (int)mp.mem[_cimg_mp_slot_z],(int)mp.mem[_cimg_mp_slot_c],(T)0);