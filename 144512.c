    **/
    inline int fdate(const char *const path, unsigned int attr) {
      int out = (int)attr;
      return fdate(path,&out,1);