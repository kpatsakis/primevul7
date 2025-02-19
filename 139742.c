DU_getStringDOElement(DcmItem *obj, DcmTagKey t, char *s, size_t bufsize)
{
    DcmByteString *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;
    char* aString;

    ec = obj->search(t, stack);
    elem = (DcmByteString*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        if (elem->getLength() == 0) {
            s[0] = '\0';
        } else {
            ec =  elem->getString(aString);
            if (ec == EC_Normal)
                OFStandard::strlcpy(s, aString, bufsize);
        }
    }
    return (ec == EC_Normal);
}