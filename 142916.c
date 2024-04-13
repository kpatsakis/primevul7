                                 const unsigned int z0, const unsigned int c0) const {
      return _save_cimg(file,0,n0,x0,y0,z0,c0);
    }

    static void _save_empty_cimg(std::FILE *const file, const char *const filename,
                                const unsigned int nb,
                                const unsigned int dx, const unsigned int dy,
                                const unsigned int dz, const unsigned int dc) {
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const ulongT siz = (ulongT)dx*dy*dz*dc*sizeof(T);
      std::fprintf(nfile,"%u %s\n",nb,pixel_type());
      for (unsigned int i=nb; i; --i) {
        std::fprintf(nfile,"%u %u %u %u\n",dx,dy,dz,dc);