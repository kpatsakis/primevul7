    void TiffParserWorker::findPrimaryGroups(PrimaryGroups& primaryGroups, TiffComponent* pSourceDir)
    {
        if (0 == pSourceDir)
            return;

        const IfdId imageGroups[] = {
            ifd0Id,
            ifd1Id,
            ifd2Id,
            ifd3Id,
            subImage1Id,
            subImage2Id,
            subImage3Id,
            subImage4Id,
            subImage5Id,
            subImage6Id,
            subImage7Id,
            subImage8Id,
            subImage9Id
        };

        for (unsigned int i = 0; i < EXV_COUNTOF(imageGroups); ++i) {
            TiffFinder finder(0x00fe, imageGroups[i]);
            pSourceDir->accept(finder);
            TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
            const Value* pV = te != nullptr ? te->pValue() : nullptr;
            if (pV && pV->typeId() == unsignedLong && pV->count() == 1 && (pV->toLong() & 1) == 0) {
                primaryGroups.push_back(te->group());
            }
        }

    } // TiffParserWorker::findPrimaryGroups