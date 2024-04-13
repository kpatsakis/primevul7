    int XmpParser::encode(      std::string& xmpPacket,
                          const XmpData&     xmpData,
                                uint16_t     formatFlags,
                                uint32_t     padding)
    { try {
        if (xmpData.empty()) {
            xmpPacket.clear();
            return 0;
        }

        if (!initialize()) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "XMP toolkit initialization failed.\n";
#endif
            return 2;
        }
        // Register custom namespaces with XMP-SDK
        for (XmpProperties::NsRegistry::iterator i = XmpProperties::nsRegistry_.begin();
             i != XmpProperties::nsRegistry_.end(); ++i) {
#ifdef DEBUG
            std::cerr << "Registering " << i->second.prefix_ << " : " << i->first << "\n";
#endif
            registerNs(i->first, i->second.prefix_);
        }
        SXMPMeta meta;
        for (XmpData::const_iterator i = xmpData.begin(); i != xmpData.end(); ++i) {
            const std::string ns = XmpProperties::ns(i->groupName());
            XMP_OptionBits options = 0;

            if (i->typeId() == langAlt) {

                // Encode Lang Alt property
                const LangAltValue* la = dynamic_cast<const LangAltValue*>(&i->value());
                if (la == 0) throw Error(kerEncodeLangAltPropertyFailed, i->key());

                int idx = 1;
                for ( LangAltValue::ValueType::const_iterator k = la->value_.begin()
                    ; k != la->value_.end()
                    ; ++k
                ) {
                    if ( k->second.size() ) { // remove lang specs with no value
                        printNode(ns, i->tagName(), k->second, 0);
                        meta.AppendArrayItem(ns.c_str(), i->tagName().c_str(), kXMP_PropArrayIsAlternate, k->second.c_str());
                        const std::string item = i->tagName() + "[" + toString(idx++) + "]";
                        meta.SetQualifier(ns.c_str(), item.c_str(), kXMP_NS_XML, "lang", k->first.c_str());
                    }
                }
                continue;
            }

            // Todo: Xmpdatum should have an XmpValue, not a Value
            const XmpValue* val = dynamic_cast<const XmpValue*>(&i->value());
            if (val == 0) throw Error(kerInvalidKeyXmpValue, i->key(), i->typeName());
            options =   xmpArrayOptionBits(val->xmpArrayType())
                      | xmpArrayOptionBits(val->xmpStruct());
            if (   i->typeId() == xmpBag
                || i->typeId() == xmpSeq
                || i->typeId() == xmpAlt) {
                printNode(ns, i->tagName(), "", options);
                meta.SetProperty(ns.c_str(), i->tagName().c_str(), 0, options);
                for (int idx = 0; idx < i->count(); ++idx) {
                    const std::string item = i->tagName() + "[" + toString(idx + 1) + "]";
                    printNode(ns, item, i->toString(idx), 0);
                    meta.SetProperty(ns.c_str(), item.c_str(), i->toString(idx).c_str());
                }
                continue;
            }
            if (i->typeId() == xmpText) {
                if (i->count() == 0) {
                    printNode(ns, i->tagName(), "", options);
                    meta.SetProperty(ns.c_str(), i->tagName().c_str(), 0, options);
                }
                else {
                    printNode(ns, i->tagName(), i->toString(0), options);
                    meta.SetProperty(ns.c_str(), i->tagName().c_str(), i->toString(0).c_str(), options);
                }
                continue;
            }
            // Don't let any Xmpdatum go by unnoticed
            throw Error(kerUnhandledXmpdatum, i->tagName(), i->typeName());
        }
        std::string tmpPacket;
        meta.SerializeToBuffer(&tmpPacket, xmpFormatOptionBits(static_cast<XmpFormatFlags>(formatFlags)), padding); // throws
        xmpPacket = tmpPacket;

        return 0;
    }
#ifndef SUPPRESS_WARNINGS
    catch (const XMP_Error& e) {
        EXV_ERROR << Error(kerXMPToolkitError, e.GetID(), e.GetErrMsg()) << "\n";
        return 3;
    }
#else
    catch (const XMP_Error&) {
        return 3;
    }
#endif // SUPPRESS_WARNINGS
    } // XmpParser::decode