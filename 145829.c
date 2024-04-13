    void TiffReader::visitIfdMakernoteEnd(TiffIfdMakernote* /*object*/)
    {
        // Reset state (byte order, create function, offset) back to that for the image
        setOrigState();
    } // TiffReader::visitIfdMakernoteEnd