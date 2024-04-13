static void squashfs_stat(char *source)
{
	time_t mkfs_time = (time_t) sBlk.s.mkfs_time;
	struct tm *t = use_localtime ? localtime(&mkfs_time) :
					gmtime(&mkfs_time);
	char *mkfs_str = asctime(t);
	long long xattr_ids = read_xattr_ids();

	if(xattr_ids == -1)
		EXIT_UNSQUASH("File system corruption detected\n");

	printf("Found a valid SQUASHFS 4:0 superblock on %s.\n", source);
	printf("Creation or last append time %s", mkfs_str ? mkfs_str :
		"failed to get time\n");
	printf("Filesystem size %llu bytes (%.2f Kbytes / %.2f Mbytes)\n",
		sBlk.s.bytes_used, sBlk.s.bytes_used / 1024.0,
		sBlk.s.bytes_used / (1024.0 * 1024.0));
	printf("Compression %s\n", comp->name);

	if(SQUASHFS_COMP_OPTS(sBlk.s.flags)) {
		char buffer[SQUASHFS_METADATA_SIZE] __attribute__ ((aligned));
		int bytes;

		if(!comp->supported)
			printf("\tCould not display compressor options, because"
				" %s compression is not supported\n",
				comp->name);
		else {
			bytes = read_block(fd, sizeof(sBlk.s), NULL, 0, buffer);
			if(bytes == 0) {
				ERROR("Failed to read compressor options\n");
				return;
			}

			compressor_display_options(comp, buffer, bytes);
		}
	}

	printf("Block size %d\n", sBlk.s.block_size);
	printf("Filesystem is %sexportable via NFS\n",
		SQUASHFS_EXPORTABLE(sBlk.s.flags) ? "" : "not ");
	printf("Inodes are %scompressed\n",
		SQUASHFS_UNCOMPRESSED_INODES(sBlk.s.flags) ? "un" : "");
	printf("Data is %scompressed\n",
		SQUASHFS_UNCOMPRESSED_DATA(sBlk.s.flags) ? "un" : "");
	printf("Uids/Gids (Id table) are %scompressed\n",
		SQUASHFS_UNCOMPRESSED_INODES(sBlk.s.flags) ||
		SQUASHFS_UNCOMPRESSED_IDS(sBlk.s.flags) ? "un" : "");

	if(SQUASHFS_NO_FRAGMENTS(sBlk.s.flags))
		printf("Fragments are not stored\n");
	else {
		printf("Fragments are %scompressed\n",
			SQUASHFS_UNCOMPRESSED_FRAGMENTS(sBlk.s.flags) ?
			"un" : "");
		printf("Always-use-fragments option is %sspecified\n",
			SQUASHFS_ALWAYS_FRAGMENTS(sBlk.s.flags) ? "" : "not ");
	}

	if(SQUASHFS_NO_XATTRS(sBlk.s.flags))
		printf("Xattrs are not stored\n");
	else
		printf("Xattrs are %scompressed\n",
			SQUASHFS_UNCOMPRESSED_XATTRS(sBlk.s.flags) ?  "un" : "");

	printf("Duplicates are %sremoved\n", SQUASHFS_DUPLICATES(sBlk.s.flags)
			? "" : "not ");
	printf("Number of fragments %u\n", sBlk.s.fragments);
	printf("Number of inodes %u\n", sBlk.s.inodes);
	printf("Number of ids %d\n", sBlk.s.no_ids);

	if(!SQUASHFS_NO_XATTRS(sBlk.s.flags))
		printf("Number of xattr ids %lld\n", xattr_ids);

	TRACE("sBlk.s.inode_table_start 0x%llx\n", sBlk.s.inode_table_start);
	TRACE("sBlk.s.directory_table_start 0x%llx\n", sBlk.s.directory_table_start);
	TRACE("sBlk.s.fragment_table_start 0x%llx\n", sBlk.s.fragment_table_start);
	TRACE("sBlk.s.lookup_table_start 0x%llx\n", sBlk.s.lookup_table_start);
	TRACE("sBlk.s.id_table_start 0x%llx\n", sBlk.s.id_table_start);
	TRACE("sBlk.s.xattr_id_table_start 0x%llx\n", sBlk.s.xattr_id_table_start);
}