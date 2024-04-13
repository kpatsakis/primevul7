    //! Load image using GraphicsMagick's external tool 'gm' \newinstance.
    static CImg<T> get_load_graphicsmagick_external(const char *const filename) {
      return CImg<T>().load_graphicsmagick_external(filename);