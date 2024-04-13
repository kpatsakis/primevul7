    template<typename t1, typename t2, typename t3, typename t4, typename t5>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const CImg<t5>& img5, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(5);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared);