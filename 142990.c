    template<typename T>
    static void screenshot(CImg<T>& img) {
      return screenshot(0,0,cimg::type<int>::max(),cimg::type<int>::max(),img);