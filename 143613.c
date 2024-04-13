    //! Load image from a .cimg[z] file \newinstance
    static CImg<T> get_load_cimg(std::FILE *const file, const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(file,axis,align);