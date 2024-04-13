    TiffCopier::TiffCopier(      TiffComponent*  pRoot,
                                 uint32_t        root,
                           const TiffHeaderBase* pHeader,
                           const PrimaryGroups*  pPrimaryGroups)
        : pRoot_(pRoot),
          root_(root),
          pHeader_(pHeader),
          pPrimaryGroups_(pPrimaryGroups)
    {
        assert(pRoot_ != 0);
        assert(pHeader_ != 0);
        assert(pPrimaryGroups_ != 0);
    }