    **/
    explicit CImgList(const char *const filename):_width(0),_allocated_width(0),_data(0) {
      assign(filename);