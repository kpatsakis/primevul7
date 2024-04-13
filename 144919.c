    template<typename T>
    static void screenshot(const int x0, const int y0, const int x1, const int y1, CImg<T>& img) {
      cimg::unused(x0,y0,x1,y1,&img);
      _no_display_exception();