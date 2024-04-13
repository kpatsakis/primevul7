      \param file File to write data to.
      \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
      \param is_rgb Tells if the RGB to YUV conversion must be done for saving.
    **/
    const CImgList<T>& save_yuv(std::FILE *const file,