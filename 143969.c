    //! Load gif file, using ImageMagick or GraphicsMagick's external tool 'convert' \newinstance.
    static CImg<T> get_load_gif_external(const char *const filename,
                                         const char axis='z', const float align=0) {
      return CImgList<T>().load_gif_external(filename).get_append(axis,align);