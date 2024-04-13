    **/
    inline double atof(const char *const str) {
      double x = 0, y = 1;
      return str && cimg_sscanf(str,"%lf/%lf",&x,&y)>0?x/y:0;