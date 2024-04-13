      // Insert code instructions for processing vectors.
      bool is_comp_vector(const unsigned int arg) const {
        unsigned int siz = _cimg_mp_size(arg);
        if (siz>8) return false;
        const int *ptr = memtype.data(arg + 1);
        bool is_tmp = true;
        while (siz-->0) if (*(ptr++)) { is_tmp = false; break; }
        return is_tmp;