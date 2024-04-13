
      static double mp_list_stats(_cimg_math_parser& mp) {
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width()),
          k = (unsigned int)mp.opcode[3];
        if (!mp.list_stats) mp.list_stats.assign(mp.listin._width);
        if (!mp.list_stats[ind]) mp.list_stats[ind].assign(1,14,1,1,0).fill(mp.listin[ind].get_stats(),false);
        return mp.list_stats(ind,k);