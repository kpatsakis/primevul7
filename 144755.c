
      void set_variable_vector(const unsigned int arg) {
        unsigned int siz = _cimg_mp_size(arg);
        int *ptr = memtype.data(arg + 1);
        while (siz-->0) *(ptr++) = -1;