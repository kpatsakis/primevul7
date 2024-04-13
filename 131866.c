PackLinuxElf64::PackLinuxElf64(InputFile *f)
    : super(f), phdri(NULL), shdri(NULL), gnu_stack(NULL), note_body(NULL),
    page_mask(~0ull<<lg2_page),
    dynseg(NULL), hashtab(NULL), gashtab(NULL), dynsym(NULL),
    jni_onload_sym(NULL),
    shstrtab(NULL),
    sec_strndx(NULL), sec_dynsym(NULL), sec_dynstr(NULL)
{
    memset(&ehdri, 0, sizeof(ehdri));
    if (f) {
        f->seek(0, SEEK_SET);
        f->readx(&ehdri, sizeof(ehdri));
    }
}