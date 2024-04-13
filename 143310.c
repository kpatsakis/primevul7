      \param align Appending alignment.
      \param[in,out] XYZ Contains the XYZ coordinates at start / exit of the function.
      \param exit_on_anykey Exit function when any key is pressed.
    **/
    const CImgList<T>& display(const char *const title=0, const bool display_info=true,
                               const char axis='x', const float align=0,
                               unsigned int *const XYZ=0, const bool exit_on_anykey=false) const {