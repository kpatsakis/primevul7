void LibarchivePlugin::emitEntryFromArchiveEntry(struct archive_entry *aentry)
{
    auto e = new Archive::Entry();

#ifdef Q_OS_WIN
    e->setProperty("fullPath", QDir::fromNativeSeparators(QString::fromUtf16((ushort*)archive_entry_pathname_w(aentry))));
#else
    e->setProperty("fullPath", QDir::fromNativeSeparators(QString::fromWCharArray(archive_entry_pathname_w(aentry))));
#endif

    const QString owner = QString::fromLatin1(archive_entry_uname(aentry));
    if (!owner.isEmpty()) {
        e->setProperty("owner", owner);
    } else {
        e->setProperty("owner", static_cast<qlonglong>(archive_entry_uid(aentry)));
    }

    const QString group = QString::fromLatin1(archive_entry_gname(aentry));
    if (!group.isEmpty()) {
        e->setProperty("group", group);
    } else {
        e->setProperty("group", static_cast<qlonglong>(archive_entry_gid(aentry)));
    }

    const mode_t mode = archive_entry_mode(aentry);
    if (mode != 0) {
        e->setProperty("permissions", QString::number(mode, 8));
    }
    e->setProperty("isExecutable", mode & (S_IXUSR | S_IXGRP | S_IXOTH));

    e->compressedSizeIsSet = false;
    e->setProperty("size", (qlonglong)archive_entry_size(aentry));
    e->setProperty("isDirectory", S_ISDIR(archive_entry_mode(aentry)));

    if (archive_entry_symlink(aentry)) {
        e->setProperty("link", QLatin1String( archive_entry_symlink(aentry) ));
    }

    auto time = static_cast<uint>(archive_entry_mtime(aentry));
    e->setProperty("timestamp", QDateTime::fromSecsSinceEpoch(time));

    emit entry(e);
    m_emittedEntries << e;
}