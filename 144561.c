    template<typename t1, typename t2>
    CImg<T>& matchpatch(const CImg<T>& patch_image,
                        const unsigned int patch_width,
                        const unsigned int patch_height,
                        const unsigned int patch_depth,
                        const unsigned int nb_iterations,
                        const unsigned int nb_randoms,
                        const float occ_penalization,
                        const CImg<t1> &guide,
                        CImg<t2> &matching_score) {
      return get_matchpatch(patch_image,patch_width,patch_height,patch_depth,
                            nb_iterations,nb_randoms,occ_penalization,guide,matching_score).move_to(*this);