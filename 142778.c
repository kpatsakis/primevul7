    **/
    static void save_empty_cimg(const char *const filename,
                                const unsigned int dx, const unsigned int dy=1,
                                const unsigned int dz=1, const unsigned int dc=1) {
      return CImgList<T>::save_empty_cimg(filename,1,dx,dy,dz,dc);