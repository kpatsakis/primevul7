       \param align Alignment setting when images have not all the same size.
       \param exit_on_anykey Exit function when any key is pressed.
       \return A one-column vector containing the selected image indexes.
    **/
    CImg<intT> get_select(const char *const title, const bool feature_type=true,
                          const char axis='x', const float align=0,