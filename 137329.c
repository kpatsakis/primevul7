doshn(struct magic_set *ms, int clazz, int swap, int fd, off_t off, int num,
    size_t size, off_t fsize, int mach, int strtab, int *flags,
    uint16_t *notecount)
{
	Elf32_Shdr sh32;
	Elf64_Shdr sh64;
	int stripped = 1;
	size_t nbadcap = 0;
	void *nbuf;
	off_t noff, coff, name_off;
	uint64_t cap_hw1 = 0;	/* SunOS 5.x hardware capabilites */
	uint64_t cap_sf1 = 0;	/* SunOS 5.x software capabilites */
	char name[50];
	ssize_t namesize;

	if (size != xsh_sizeof) {
		if (file_printf(ms, ", corrupted section header size") == -1)
			return -1;
		return 0;
	}

	/* Read offset of name section to be able to read section names later */
	if (pread(fd, xsh_addr, xsh_sizeof, CAST(off_t, (off + size * strtab)))
	    < (ssize_t)xsh_sizeof) {
		file_badread(ms);
		return -1;
	}
	name_off = xsh_offset;

	for ( ; num; num--) {
		/* Read the name of this section. */
		if ((namesize = pread(fd, name, sizeof(name) - 1, name_off + xsh_name)) == -1) {
			file_badread(ms);
			return -1;
		}
		name[namesize] = '\0';
		if (strcmp(name, ".debug_info") == 0)
			stripped = 0;

		if (pread(fd, xsh_addr, xsh_sizeof, off) < (ssize_t)xsh_sizeof) {
			file_badread(ms);
			return -1;
		}
		off += size;

		/* Things we can determine before we seek */
		switch (xsh_type) {
		case SHT_SYMTAB:
#if 0
		case SHT_DYNSYM:
#endif
			stripped = 0;
			break;
		default:
			if (fsize != SIZE_UNKNOWN && xsh_offset > fsize) {
				/* Perhaps warn here */
				continue;
			}
			break;
		}


		/* Things we can determine when we seek */
		switch (xsh_type) {
		case SHT_NOTE:
			if ((uintmax_t)(xsh_size + xsh_offset) >
			    (uintmax_t)fsize) {
				if (file_printf(ms,
				    ", note offset/size 0x%" INTMAX_T_FORMAT
				    "x+0x%" INTMAX_T_FORMAT "x exceeds"
				    " file size 0x%" INTMAX_T_FORMAT "x",
				    (uintmax_t)xsh_offset, (uintmax_t)xsh_size,
				    (uintmax_t)fsize) == -1)
					return -1;
				return 0; 
			}
			if ((nbuf = malloc(xsh_size)) == NULL) {
				file_error(ms, errno, "Cannot allocate memory"
				    " for note");
				return -1;
			}
			if (pread(fd, nbuf, xsh_size, xsh_offset) <
			    (ssize_t)xsh_size) {
				file_badread(ms);
				free(nbuf);
				return -1;
			}

			noff = 0;
			for (;;) {
				if (noff >= (off_t)xsh_size)
					break;
				noff = donote(ms, nbuf, (size_t)noff,
				    xsh_size, clazz, swap, 4, flags, notecount,
				    fd, 0, 0, 0);
				if (noff == 0)
					break;
			}
			free(nbuf);
			break;
		case SHT_SUNW_cap:
			switch (mach) {
			case EM_SPARC:
			case EM_SPARCV9:
			case EM_IA_64:
			case EM_386:
			case EM_AMD64:
				break;
			default:
				goto skip;
			}

			if (nbadcap > 5)
				break;
			if (lseek(fd, xsh_offset, SEEK_SET) == (off_t)-1) {
				file_badseek(ms);
				return -1;
			}
			coff = 0;
			for (;;) {
				Elf32_Cap cap32;
				Elf64_Cap cap64;
				char cbuf[/*CONSTCOND*/
				    MAX(sizeof cap32, sizeof cap64)];
				if ((coff += xcap_sizeof) > (off_t)xsh_size)
					break;
				if (read(fd, cbuf, (size_t)xcap_sizeof) !=
				    (ssize_t)xcap_sizeof) {
					file_badread(ms);
					return -1;
				}
				if (cbuf[0] == 'A') {
#ifdef notyet
					char *p = cbuf + 1;
					uint32_t len, tag;
					memcpy(&len, p, sizeof(len));
					p += 4;
					len = getu32(swap, len);
					if (memcmp("gnu", p, 3) != 0) {
					    if (file_printf(ms,
						", unknown capability %.3s", p)
						== -1)
						return -1;
					    break;
					}
					p += strlen(p) + 1;
					tag = *p++;
					memcpy(&len, p, sizeof(len));
					p += 4;
					len = getu32(swap, len);
					if (tag != 1) {
					    if (file_printf(ms, ", unknown gnu"
						" capability tag %d", tag)
						== -1)
						return -1;
					    break;
					}
					// gnu attributes 
#endif
					break;
				}
				(void)memcpy(xcap_addr, cbuf, xcap_sizeof);
				switch (xcap_tag) {
				case CA_SUNW_NULL:
					break;
				case CA_SUNW_HW_1:
					cap_hw1 |= xcap_val;
					break;
				case CA_SUNW_SF_1:
					cap_sf1 |= xcap_val;
					break;
				default:
					if (file_printf(ms,
					    ", with unknown capability "
					    "0x%" INT64_T_FORMAT "x = 0x%"
					    INT64_T_FORMAT "x",
					    (unsigned long long)xcap_tag,
					    (unsigned long long)xcap_val) == -1)
						return -1;
					if (nbadcap++ > 2)
						coff = xsh_size;
					break;
				}
			}
			/*FALLTHROUGH*/
		skip:
		default:
			break;
		}
	}

	if (file_printf(ms, ", %sstripped", stripped ? "" : "not ") == -1)
		return -1;
	if (cap_hw1) {
		const cap_desc_t *cdp;
		switch (mach) {
		case EM_SPARC:
		case EM_SPARC32PLUS:
		case EM_SPARCV9:
			cdp = cap_desc_sparc;
			break;
		case EM_386:
		case EM_IA_64:
		case EM_AMD64:
			cdp = cap_desc_386;
			break;
		default:
			cdp = NULL;
			break;
		}
		if (file_printf(ms, ", uses") == -1)
			return -1;
		if (cdp) {
			while (cdp->cd_name) {
				if (cap_hw1 & cdp->cd_mask) {
					if (file_printf(ms,
					    " %s", cdp->cd_name) == -1)
						return -1;
					cap_hw1 &= ~cdp->cd_mask;
				}
				++cdp;
			}
			if (cap_hw1)
				if (file_printf(ms,
				    " unknown hardware capability 0x%"
				    INT64_T_FORMAT "x",
				    (unsigned long long)cap_hw1) == -1)
					return -1;
		} else {
			if (file_printf(ms,
			    " hardware capability 0x%" INT64_T_FORMAT "x",
			    (unsigned long long)cap_hw1) == -1)
				return -1;
		}
	}
	if (cap_sf1) {
		if (cap_sf1 & SF1_SUNW_FPUSED) {
			if (file_printf(ms,
			    (cap_sf1 & SF1_SUNW_FPKNWN)
			    ? ", uses frame pointer"
			    : ", not known to use frame pointer") == -1)
				return -1;
		}
		cap_sf1 &= ~SF1_SUNW_MASK;
		if (cap_sf1)
			if (file_printf(ms,
			    ", with unknown software capability 0x%"
			    INT64_T_FORMAT "x",
			    (unsigned long long)cap_sf1) == -1)
				return -1;
	}
	return 0;
}