    **/
    explicit CImgList(const CImgDisplay& disp):_width(0),_allocated_width(0),_data(0) {
      assign(disp);