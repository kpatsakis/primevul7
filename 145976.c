    WriteMethod ExifParser::encode(
              Blob&     blob,
        const byte*     pData,
              uint32_t  size,
              ByteOrder byteOrder,
        const ExifData& exifData
    )
    {
        ExifData ed = exifData;

        // Delete IFD0 tags that are "not recorded" in compressed images
        // Reference: Exif 2.2 specs, 4.6.8 Tag Support Levels, section A
        static const char* filteredIfd0Tags[] = {
            "Exif.Image.PhotometricInterpretation",
            "Exif.Image.StripOffsets",
            "Exif.Image.RowsPerStrip",
            "Exif.Image.StripByteCounts",
            "Exif.Image.JPEGInterchangeFormat",
            "Exif.Image.JPEGInterchangeFormatLength",
            "Exif.Image.SubIFDs"
        };
        for (unsigned int i = 0; i < EXV_COUNTOF(filteredIfd0Tags); ++i) {
            ExifData::iterator pos = ed.findKey(ExifKey(filteredIfd0Tags[i]));
            if (pos != ed.end()) {
#ifdef DEBUG
                std::cerr << "Warning: Exif tag " << pos->key() << " not encoded\n";
#endif
                ed.erase(pos);
            }
        }

        // Delete IFDs which do not occur in JPEGs
        static const IfdId filteredIfds[] = {
            subImage1Id,
            subImage2Id,
            subImage3Id,
            subImage4Id,
            subImage5Id,
            subImage6Id,
            subImage7Id,
            subImage8Id,
            subImage9Id,
            subThumb1Id,
            panaRawId,
            ifd2Id,
            ifd3Id
        };
        for (unsigned int i = 0; i < EXV_COUNTOF(filteredIfds); ++i) {
#ifdef DEBUG
            std::cerr << "Warning: Exif IFD " << filteredIfds[i] << " not encoded\n";
#endif
            eraseIfd(ed, filteredIfds[i]);
        }

        // IPTC and XMP are stored elsewhere, not in the Exif APP1 segment.
        IptcData emptyIptc;
        XmpData  emptyXmp;

        // Encode and check if the result fits into a JPEG Exif APP1 segment
        MemIo mio1;
        std::unique_ptr<TiffHeaderBase> header(new TiffHeader(byteOrder, 0x00000008, false));
        WriteMethod wm = TiffParserWorker::encode(mio1,
                                                  pData,
                                                  size,
                                                  ed,
                                                  emptyIptc,
                                                  emptyXmp,
                                                  Tag::root,
                                                  TiffMapping::findEncoder,
                                                  header.get(),
                                                  0);
        if (mio1.size() <= 65527) {
            append(blob, mio1.mmap(), (uint32_t) mio1.size());
            return wm;
        }

        // If it doesn't fit, remove additional tags

        // Delete preview tags if the preview is larger than 32kB.
        // Todo: Enhance preview classes to be able to write and delete previews and use that instead.
        // Table must be sorted by preview, the first tag in each group is the size
        static const PreviewTags filteredPvTags[] = {
            { pttLen, "Exif.Minolta.ThumbnailLength"                  },
            { pttTag, "Exif.Minolta.ThumbnailOffset"                  },
            { pttLen, "Exif.Minolta.Thumbnail"                        },
            { pttLen, "Exif.NikonPreview.JPEGInterchangeFormatLength" },
            { pttIfd, "NikonPreview"                                  },
            { pttLen, "Exif.Olympus.ThumbnailLength"                  },
            { pttTag, "Exif.Olympus.ThumbnailOffset"                  },
            { pttLen, "Exif.Olympus.ThumbnailImage"                   },
            { pttLen, "Exif.Olympus.Thumbnail"                        },
            { pttLen, "Exif.Olympus2.ThumbnailLength"                 },
            { pttTag, "Exif.Olympus2.ThumbnailOffset"                 },
            { pttLen, "Exif.Olympus2.ThumbnailImage"                  },
            { pttLen, "Exif.Olympus2.Thumbnail"                       },
            { pttLen, "Exif.OlympusCs.PreviewImageLength"             },
            { pttTag, "Exif.OlympusCs.PreviewImageStart"              },
            { pttTag, "Exif.OlympusCs.PreviewImageValid"              },
            { pttLen, "Exif.Pentax.PreviewLength"                     },
            { pttTag, "Exif.Pentax.PreviewOffset"                     },
            { pttTag, "Exif.Pentax.PreviewResolution"                 },
            { pttLen, "Exif.PentaxDng.PreviewLength"                  },
            { pttTag, "Exif.PentaxDng.PreviewOffset"                  },
            { pttTag, "Exif.PentaxDng.PreviewResolution"              },
            { pttLen, "Exif.SamsungPreview.JPEGInterchangeFormatLength" },
            { pttIfd, "SamsungPreview"                                },
            { pttLen, "Exif.Thumbnail.StripByteCounts"                },
            { pttIfd, "Thumbnail"                                     },
            { pttLen, "Exif.Thumbnail.JPEGInterchangeFormatLength"    },
            { pttIfd, "Thumbnail"                                     }
        };
        bool delTags = false;
        ExifData::iterator pos;
        for (unsigned int i = 0; i < EXV_COUNTOF(filteredPvTags); ++i) {
            switch (filteredPvTags[i].ptt_) {
            case pttLen:
                delTags = false;
                pos = ed.findKey(ExifKey(filteredPvTags[i].key_));
                if (pos != ed.end() && sumToLong(*pos) > 32768) {
                    delTags = true;
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Exif tag " << pos->key() << " not encoded\n";
#endif
                    ed.erase(pos);
                }
                break;
            case pttTag:
                if (delTags) {
                    pos = ed.findKey(ExifKey(filteredPvTags[i].key_));
                    if (pos != ed.end()) {
#ifndef SUPPRESS_WARNINGS
                        EXV_WARNING << "Exif tag " << pos->key() << " not encoded\n";
#endif
                        ed.erase(pos);
                    }
                }
                break;
            case pttIfd:
                if (delTags) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Exif IFD " << filteredPvTags[i].key_ << " not encoded\n";
#endif
                    eraseIfd(ed, Internal::groupId(filteredPvTags[i].key_));
                }
                break;
            }
        }

        // Delete unknown tags larger than 4kB and known tags larger than 20kB.
        for (ExifData::iterator tag_iter = ed.begin(); tag_iter != ed.end(); ) {
            if ( (tag_iter->size() > 4096 && tag_iter->tagName().substr(0, 2) == "0x") ||
                  tag_iter->size() > 20480) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Exif tag " << tag_iter->key() << " not encoded\n";
#endif
                tag_iter = ed.erase(tag_iter);
            }
            else {
                ++tag_iter;
            }
        }

        // Encode the remaining Exif tags again, don't care if it fits this time
        MemIo mio2;
        wm = TiffParserWorker::encode(mio2,
                                      pData,
                                      size,
                                      ed,
                                      emptyIptc,
                                      emptyXmp,
                                      Tag::root,
                                      TiffMapping::findEncoder,
                                      header.get(),
                                      0);
        append(blob, mio2.mmap(), (uint32_t) mio2.size());
#ifdef DEBUG
        if (wm == wmIntrusive) {
            std::cerr << "SIZE OF EXIF DATA IS " << std::dec << mio2.size() << " BYTES\n";
        }
        else {
            std::cerr << "SIZE DOESN'T MATTER, NON-INTRUSIVE WRITING USED\n";
        }
#endif
        return wm;

    }