    bool TiffHeader::isImageTag(      uint16_t       tag,
                                      IfdId          group,
                                const PrimaryGroups* pPrimaryGroups) const
    {
        if (!hasImageTags_) {
#ifdef DEBUG
            std::cerr << "No image tags in this image\n";
#endif
            return false;
        }
#ifdef DEBUG
        ExifKey key(tag, groupName(group));
#endif
        // If there are primary groups and none matches group, we're done
        if (   pPrimaryGroups != 0
            && !pPrimaryGroups->empty()
            && std::find(pPrimaryGroups->begin(), pPrimaryGroups->end(), group)
               == pPrimaryGroups->end()) {
#ifdef DEBUG
            std::cerr << "Not an image tag: " << key << " (1)\n";
#endif
            return false;
        }
        // All tags of marked primary groups other than IFD0 are considered
        // image tags. That should take care of NEFs until we know better.
        if (   pPrimaryGroups != 0
            && !pPrimaryGroups->empty()
            && group != ifd0Id) {
#ifdef DEBUG
            ExifKey key(tag, groupName(group));
            std::cerr << "Image tag: " << key << " (2)\n";
#endif
            return true;
        }
        // Finally, if tag, group is one of the TIFF image tags -> bingo!
        return isTiffImageTag(tag, group);
    } // TiffHeader::isImageTag