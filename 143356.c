    **/
    static const CImg<Tuchar>& cool_LUT256() {
      static CImg<Tuchar> colormap;
      cimg::mutex(8);
      if (!colormap) colormap.assign(1,2,1,3).fill((T)0,(T)255,(T)255,(T)0,(T)255,(T)255).resize(1,256,1,3,3);
      cimg::mutex(8,0);
      return colormap;