    //! Load image from a PAR-REC (Philips) file \newinstance.
    static CImg<T> get_load_parrec(const char *const filename, const char axis='c', const float align=0) {
      return CImg<T>().load_parrec(filename,axis,align);