    template<typename T>
    inline size_t fwrite(const T *ptr, const size_t nmemb, std::FILE *stream) {
      if (!ptr || !stream)
        throw CImgArgumentException("cimg::fwrite(): Invalid writing request of %u %s%s from buffer %p to file %p.",
                                    nmemb,cimg::type<T>::string(),nmemb>1?"s":"",ptr,stream);
      if (!nmemb) return 0;
      const size_t wlimitT = 63*1024*1024, wlimit = wlimitT/sizeof(T);
      size_t to_write = nmemb, al_write = 0, l_to_write = 0, l_al_write = 0;
      do {
        l_to_write = (to_write*sizeof(T))<wlimitT?to_write:wlimit;
        l_al_write = std::fwrite((void*)(ptr + al_write),sizeof(T),l_to_write,stream);
        al_write+=l_al_write;
        to_write-=l_al_write;
      } while (l_to_write==l_al_write && to_write>0);
      if (to_write>0)
        warn("cimg::fwrite(): Only %lu/%lu elements could be written in file.",
             (unsigned long)al_write,(unsigned long)nmemb);
      return al_write;