    TiffBinaryArray::TiffBinaryArray(uint16_t tag,
                                     IfdId group,
                                     const ArraySet* arraySet,
                                     int setSize,
                                     CfgSelFct cfgSelFct)
        : TiffEntryBase(tag, group), // Todo: Does it make a difference that there is no type?
          cfgSelFct_(cfgSelFct),
          arraySet_(arraySet),
          arrayCfg_(0),
          arrayDef_(0),
          defSize_(0),
          setSize_(setSize),
          origData_(0),
          origSize_(0),
          pRoot_(0),
          decoded_(false)
    {
        // We'll figure out the correct cfg later
        assert(cfgSelFct != 0);
        assert(arraySet_ != 0);
    }