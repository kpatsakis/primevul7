      return get_split(axis,nb).move_to(*this);
    }

    //! Return a list where each image has been split along the specified axis \newinstance.
    CImgList<T> get_split(const char axis, const int nb=-1) const {