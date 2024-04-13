qemuPrepareNVRAM(virQEMUDriverConfigPtr cfg,
                 virDomainObjPtr vm)
{
    int ret = -1;
    int srcFD = -1;
    int dstFD = -1;
    virDomainLoaderDefPtr loader = vm->def->os.loader;
    bool created = false;
    const char *master_nvram_path;
    ssize_t r;

    if (!loader || !loader->nvram || virFileExists(loader->nvram))
        return 0;

    master_nvram_path = loader->templt;
    if (!loader->templt) {
        size_t i;
        for (i = 0; i < cfg->nfirmwares; i++) {
            if (STREQ(cfg->firmwares[i]->name, loader->path)) {
                master_nvram_path = cfg->firmwares[i]->nvram;
                break;
            }
        }
    }

    if (!master_nvram_path) {
        virReportError(VIR_ERR_OPERATION_FAILED,
                       _("unable to find any master var store for "
                         "loader: %s"), loader->path);
        goto cleanup;
    }

    if ((srcFD = virFileOpenAs(master_nvram_path, O_RDONLY,
                               0, -1, -1, 0)) < 0) {
        virReportSystemError(-srcFD,
                             _("Failed to open file '%s'"),
                             master_nvram_path);
        goto cleanup;
    }
    if ((dstFD = virFileOpenAs(loader->nvram,
                               O_WRONLY | O_CREAT | O_EXCL,
                               S_IRUSR | S_IWUSR,
                               cfg->user, cfg->group, 0)) < 0) {
        virReportSystemError(-dstFD,
                             _("Failed to create file '%s'"),
                             loader->nvram);
        goto cleanup;
    }
    created = true;

    do {
        char buf[1024];

        if ((r = saferead(srcFD, buf, sizeof(buf))) < 0) {
            virReportSystemError(errno,
                                 _("Unable to read from file '%s'"),
                                 master_nvram_path);
            goto cleanup;
        }

        if (safewrite(dstFD, buf, r) < 0) {
            virReportSystemError(errno,
                                 _("Unable to write to file '%s'"),
                                 loader->nvram);
            goto cleanup;
        }
    } while (r);

    if (VIR_CLOSE(srcFD) < 0) {
        virReportSystemError(errno,
                             _("Unable to close file '%s'"),
                             master_nvram_path);
        goto cleanup;
    }
    if (VIR_CLOSE(dstFD) < 0) {
        virReportSystemError(errno,
                             _("Unable to close file '%s'"),
                             loader->nvram);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    /* We successfully generated the nvram path, but failed to
     * copy the file content. Roll back. */
    if (ret < 0) {
        if (created)
            unlink(loader->nvram);
    }

    VIR_FORCE_CLOSE(srcFD);
    VIR_FORCE_CLOSE(dstFD);
    return ret;
}