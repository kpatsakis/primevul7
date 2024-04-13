    bool TiffHeaderBase::isImageTag(      uint16_t       /*tag*/,
                                          IfdId          /*group*/,
                                    const PrimaryGroups* /*primaryGroups*/) const
    {
        return false;
    }