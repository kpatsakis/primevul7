    void TiffEntryBase::encode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        doEncode(encoder, datum);
    } // TiffComponent::encode