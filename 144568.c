    //! Load sub-images of a .cimg file \newinstance.
    static CImg<T> get_load_cimg(const char *const filename,
                                 const unsigned int n0, const unsigned int n1,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0,
                                 const unsigned int x1, const unsigned int y1,
                                 const unsigned int z1, const unsigned int c1,
                                 const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(filename,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1,axis,align);