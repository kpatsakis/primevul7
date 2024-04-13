       \param pos Index of the insertion.
       \param is_shared Tells if inserted images are shared copies of \c img or not.
    **/
    template<typename t>
    CImgList<T>& insert(const unsigned int n, const CImg<t>& img, const unsigned int pos=~0U,
                        const bool is_shared=false) {
      if (!n) return *this;
      const unsigned int npos = pos==~0U?_width:pos;