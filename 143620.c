    **/
    const CImg<T>& save_yuv(std::FILE *const file,
                            const unsigned int chroma_subsampling=444,
                            const bool is_rgb=true) const {
      CImgList<T>(*this,true).save_yuv(file,chroma_subsampling,is_rgb);
      return *this;