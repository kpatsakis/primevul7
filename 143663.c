    template<typename T>
    CImgDisplay& assign(const CImgList<T>& list, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool fullscreen_flag=false, const bool closed_flag=false) {
      if (!list) return assign();
      CImg<T> tmp;
      const CImg<T> img = list>'x', &nimg = (img._depth==1)?img:(tmp=img.get_projections2d((img._width - 1)/2,
                                                                                           (img._height - 1)/2,
                                                                                           (img._depth - 1)/2));
      _assign(nimg._width,nimg._height,title,normalization_type,fullscreen_flag,closed_flag);
      if (_normalization==2) _min = (float)nimg.min_max(_max);
      return display(nimg);