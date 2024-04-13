    void TiffDecoder::visitIfdMakernote(TiffIfdMakernote* object)
    {
        assert(object != 0);

        exifData_["Exif.MakerNote.Offset"] = object->mnOffset();
        switch (object->byteOrder()) {
        case littleEndian:
            exifData_["Exif.MakerNote.ByteOrder"] = "II";
            break;
        case bigEndian:
            exifData_["Exif.MakerNote.ByteOrder"] = "MM";
            break;
        case invalidByteOrder:
            assert(object->byteOrder() != invalidByteOrder);
            break;
        }
    }