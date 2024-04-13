    template<typename t>
    CImg<T>& matchpatch(const CImg<T>& patch_image,
                        const unsigned int patch_width,
                        const unsigned int patch_height,
                        const unsigned int patch_depth,
                        const unsigned int nb_iterations=5,
                        const unsigned int nb_randoms=5,
                        const float occ_penalization=0,
                        const CImg<t> &guide=CImg<t>::const_empty()) {
      return get_matchpatch(patch_image,patch_width,patch_height,patch_depth,
                            nb_iterations,nb_randoms,occ_penalization,guide).move_to(*this);