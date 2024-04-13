      \param pos Index of the insertion.
      \param is_shared Tells if inserted images are shared copies of images of \c list or not.
    **/
    template<typename t>
    CImgList<T>& insert(const CImgList<t>& list, const unsigned int pos=~0U, const bool is_shared=false) {
      const unsigned int npos = pos==~0U?_width:pos;