void PackLinuxElf64::pack1(OutputFile *fo, Filter & /*ft*/)
{
    fi->seek(0, SEEK_SET);
    fi->readx(&ehdri, sizeof(ehdri));
    assert(e_phoff == sizeof(Elf64_Ehdr));  // checked by canPack()
    sz_phdrs = e_phnum * get_te16(&ehdri.e_phentsize);

    Elf64_Phdr const *phdr = phdri;
    note_size = 0;
    for (unsigned j=0; j < e_phnum; ++phdr, ++j) {
        if (phdr->PT_NOTE64 == get_te32(&phdr->p_type)) {
            note_size += up4(get_te64(&phdr->p_filesz));
        }
    }
    if (note_size) {
        note_body = New(unsigned char, note_size);
        note_size = 0;
    }
    phdr = phdri;
    for (unsigned j=0; j < e_phnum; ++phdr, ++j) {
        unsigned const type = get_te32(&phdr->p_type);
        if (phdr->PT_NOTE64 == type) {
            unsigned const len = get_te64(&phdr->p_filesz);
            fi->seek(get_te64(&phdr->p_offset), SEEK_SET);
            fi->readx(&note_body[note_size], len);
            note_size += up4(len);
        }
        if (phdr->PT_LOAD64 == type) {
            unsigned x = get_te64(&phdr->p_align) >> lg2_page;
            while (x>>=1) {
                ++lg2_page;
            }
        }
        if (phdr->PT_GNU_STACK64 == type) {
            gnu_stack = phdr;
        }
    }
    page_size =  1u  <<lg2_page;
    page_mask = ~0ull<<lg2_page;

    progid = 0;  // getRandomId();  not useful, so do not clutter
    if (0!=xct_off) {  // shared library
        fi->seek(0, SEEK_SET);
        fi->readx(ibuf, xct_off);

        sz_elf_hdrs = xct_off;
        fo->write(ibuf, xct_off);
        memset(&linfo, 0, sizeof(linfo));
        fo->write(&linfo, sizeof(linfo));
    }

    // only execute if option present
    if (opt->o_unix.preserve_build_id) {
        // set this so we can use elf_find_section_name
        e_shnum = ehdri.e_shnum;

        // there is a class member similar to this, but I did not
        // want to assume it would be available
        Elf64_Shdr const *tmp = shdri;

        Elf64_Shdr *shdr = NULL;

        if (! shdri) {
            shdr = New(Elf64_Shdr, e_shnum);

            fi->seek(0,SEEK_SET);
            fi->seek(ehdri.e_shoff,SEEK_SET);
            fi->readx((void*)shdr,ehdri.e_shentsize*ehdri.e_shnum);

            // set this so we can use elf_find_section_name
            shdri = (Elf64_Shdr *)shdr;
        }

        //set the shstrtab
        sec_strndx = &shdri[ehdri.e_shstrndx];

        char *strtab = New(char, sec_strndx->sh_size);
        fi->seek(0,SEEK_SET);
        fi->seek(sec_strndx->sh_offset,SEEK_SET);
        fi->readx(strtab,sec_strndx->sh_size);

        shstrtab = (const char*)strtab;

        Elf64_Shdr const *buildid = elf_find_section_name(".note.gnu.build-id");
        if (buildid) {
            unsigned char *data = New(unsigned char, buildid->sh_size);
            memset(data,0,buildid->sh_size);
            fi->seek(0,SEEK_SET);
            fi->seek(buildid->sh_offset,SEEK_SET);
            fi->readx(data,buildid->sh_size);

            buildid_data  = data;

            o_elf_shnum = 3;
            memset(&shdrout,0,sizeof(shdrout));

            //setup the build-id
            memcpy(&shdrout.shdr[1],buildid, sizeof(shdrout.shdr[1]));
            shdrout.shdr[1].sh_name = 1;

            //setup the shstrtab
            memcpy(&shdrout.shdr[2],sec_strndx, sizeof(shdrout.shdr[2]));
            shdrout.shdr[2].sh_name = 20;
            shdrout.shdr[2].sh_size = 29; //size of our static shstrtab
        }

        if (shdr) {
            shdri = tmp;
            delete [] shdr;
            shdr = NULL;
        }
    }
}