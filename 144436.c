    //! Return a list with elements being shared copies of images in the list instance \const.
    const CImgList<T> get_shared() const {
      CImgList<T> res(_width);
      cimglist_for(*this,l) res[l].assign(_data[l],true);
      return res;