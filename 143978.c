    //! Load image from a .cimg[z] file \newinstance
    static CImg<T> get_load_cimg(const char *const filename, const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(filename,axis,align);