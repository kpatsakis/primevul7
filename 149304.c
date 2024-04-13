donote(struct magic_set *ms, void *vbuf, size_t offset, size_t size,
    int clazz, int swap, size_t align, int *flags, uint16_t *notecount,
    int fd, off_t ph_off, int ph_num, off_t fsize)
{
	Elf32_Nhdr nh32;
	Elf64_Nhdr nh64;
	size_t noff, doff;
	uint32_t namesz, descsz;
	unsigned char *nbuf = CAST(unsigned char *, vbuf);

	if (*notecount == 0)
		return 0;
	--*notecount;

	if (xnh_sizeof + offset > size) {
		/*
		 * We're out of note headers.
		 */
		return xnh_sizeof + offset;
	}

	memcpy(xnh_addr, &nbuf[offset], xnh_sizeof);
	offset += xnh_sizeof;

	namesz = xnh_namesz;
	descsz = xnh_descsz;

	if ((namesz == 0) && (descsz == 0)) {
		/*
		 * We're out of note headers.
		 */
		return (offset >= size) ? offset : size;
	}

	if (namesz & 0x80000000) {
		if (file_printf(ms, ", bad note name size %#lx",
		    CAST(unsigned long, namesz)) == -1)
			return 0;
	    return 0;
	}

	if (descsz & 0x80000000) {
		if (file_printf(ms, ", bad note description size %#lx",
		    CAST(unsigned long, descsz)) == -1)
		    	return 0;
	    return 0;
	}

	noff = offset;
	doff = ELF_ALIGN(offset + namesz);

	if (offset + namesz > size) {
		/*
		 * We're past the end of the buffer.
		 */
		return doff;
	}

	offset = ELF_ALIGN(doff + descsz);
	if (doff + descsz > size) {
		/*
		 * We're past the end of the buffer.
		 */
		return (offset >= size) ? offset : size;
	}


	if ((*flags & FLAGS_DID_OS_NOTE) == 0) {
		if (do_os_note(ms, nbuf, xnh_type, swap,
		    namesz, descsz, noff, doff, flags))
			return offset;
	}

	if ((*flags & FLAGS_DID_BUILD_ID) == 0) {
		if (do_bid_note(ms, nbuf, xnh_type, swap,
		    namesz, descsz, noff, doff, flags))
			return offset;
	}

	if ((*flags & FLAGS_DID_NETBSD_PAX) == 0) {
		if (do_pax_note(ms, nbuf, xnh_type, swap,
		    namesz, descsz, noff, doff, flags))
			return offset;
	}

	if ((*flags & FLAGS_DID_CORE) == 0) {
		if (do_core_note(ms, nbuf, xnh_type, swap,
		    namesz, descsz, noff, doff, flags, size, clazz))
			return offset;
	}

	if ((*flags & FLAGS_DID_AUXV) == 0) {
		if (do_auxv_note(ms, nbuf, xnh_type, swap,
			namesz, descsz, noff, doff, flags, size, clazz,
			fd, ph_off, ph_num, fsize))
			return offset;
	}

	if (namesz == 7 && strcmp(RCAST(char *, &nbuf[noff]), "NetBSD") == 0) {
		int descw, flag;
		const char *str, *tag;
		if (descsz > 100)
			descsz = 100;
		switch (xnh_type) {
	    	case NT_NETBSD_VERSION:
			return offset;
		case NT_NETBSD_MARCH:
			flag = FLAGS_DID_NETBSD_MARCH;
			tag = "compiled for";
			break;
		case NT_NETBSD_CMODEL:
			flag = FLAGS_DID_NETBSD_CMODEL;
			tag = "compiler model";
			break;
		case NT_NETBSD_EMULATION:
			flag = FLAGS_DID_NETBSD_EMULATION;
			tag = "emulation:";
			break;
		default:
			if (*flags & FLAGS_DID_NETBSD_UNKNOWN)
				return offset;
			*flags |= FLAGS_DID_NETBSD_UNKNOWN;
			if (file_printf(ms, ", note=%u", xnh_type) == -1)
				return offset;
			return offset;
		}

		if (*flags & flag)
			return offset;
		str = RCAST(const char *, &nbuf[doff]);
		descw = CAST(int, descsz);
		*flags |= flag;
		file_printf(ms, ", %s: %.*s", tag, descw, str);
		return offset;
	}

	return offset;
}