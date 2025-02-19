    AccessMode ImageFactory::checkMode(ImageType type, MetadataId metadataId)
    {
        const Registry* r = find(registry, type);
        if (!r)
            throw Error(kerUnsupportedImageType, static_cast<int>(type));
        AccessMode am = amNone;
        switch (metadataId) {
        case mdNone:
            break;
        case mdExif:
            am = r->exifSupport_;
            break;
        case mdIptc:
            am = r->iptcSupport_;
            break;
        case mdXmp:
            am = r->xmpSupport_;
            break;
        case mdComment:
            am = r->commentSupport_;
            break;
        case mdIccProfile: break;

        // no default: let the compiler complain
        }
        return am;
    }