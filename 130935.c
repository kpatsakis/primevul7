__fixup_rootseek(zzip_off_t offset_of_trailer, struct _disk_trailer *trailer)
{
    if (_disk_trailer_rootseek(trailer) >
        offset_of_trailer - _disk_trailer_rootsize(trailer) &&
        offset_of_trailer > _disk_trailer_rootsize(trailer))
    {
        register zzip_off_t offset;
        offset = offset_of_trailer - _disk_trailer_rootsize(trailer);
        _disk_trailer_set_rootseek(trailer, offset);
        HINT2("new rootseek=%li", (long) _disk_trailer_rootseek(trailer));
    }
}