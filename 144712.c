    template<typename t>
    explicit CImgList(const CImg<t>& img, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(1);
      _data[0].assign(img,is_shared);