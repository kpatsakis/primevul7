    template<typename t>
    CImgDisplay& operator=(const CImgList<t>& list) {
      return display(list);