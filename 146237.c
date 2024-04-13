    void TiffEncoder::add(
        TiffComponent* pRootDir,
        TiffComponent* pSourceDir,
        uint32_t       root
    )
    {
        assert(pRootDir != 0);

        writeMethod_ = wmIntrusive;
        pSourceTree_ = pSourceDir;

        // Ensure that the exifData_ entries are not deleted, to be able to
        // iterate over all remaining entries.
        del_ = false;

        ExifData::const_iterator posBo = exifData_.end();
        for (ExifData::const_iterator i = exifData_.begin();
             i != exifData_.end(); ++i) {

            IfdId group = groupId(i->groupName());
            // Skip synthesized info tags
            if (group == mnId) {
                if (i->tag() == 0x0002) {
                    posBo = i;
                }
                continue;
            }

            // Skip image tags of existing TIFF image - they were copied earlier -
            // but add and encode image tags of new images (creation)
            if (isImageTag(i->tag(), group)) continue;

            // Assumption is that the corresponding TIFF entry doesn't exist
            TiffPath tiffPath;
            TiffCreator::getPath(tiffPath, i->tag(), group, root);
            TiffComponent* tc = pRootDir->addPath(i->tag(), tiffPath, pRootDir);
            TiffEntryBase* object = dynamic_cast<TiffEntryBase*>(tc);
#ifdef DEBUG
            if (object == 0) {
                std::cerr << "Warning: addPath() didn't add an entry for "
                          << i->groupName()
                          << " tag 0x" << std::setw(4) << std::setfill('0')
                          << std::hex << i->tag() << "\n";
            }
#endif
            if (object != 0) {
                encodeTiffComponent(object, &(*i));
            }
        }

        /*
          What follows is a hack. I can't think of a better way to set
          the makernote byte order (and other properties maybe) in the
          makernote header during intrusive writing. The thing is that
          visit/encodeIfdMakernote is not called in this case and there
          can't be an Exif tag which corresponds to this component.
         */
        if (posBo == exifData_.end()) return;

        TiffFinder finder(0x927c, exifId);
        pRootDir->accept(finder);
        TiffMnEntry* te = dynamic_cast<TiffMnEntry*>(finder.result());
        if (te) {
            TiffIfdMakernote* tim = dynamic_cast<TiffIfdMakernote*>(te->mn_);
            if (tim) {
                // Set Makernote byte order
                ByteOrder bo = stringToByteOrder(posBo->toString());
                if (bo != invalidByteOrder) tim->setByteOrder(bo);
            }
        }

    } // TiffEncoder::add