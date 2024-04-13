jbig2_symbol_dictionary(Jbig2Ctx *ctx, Jbig2Segment *segment, const byte *segment_data)
{
    Jbig2SymbolDictParams params;
    uint16_t flags;
    uint32_t sdat_bytes;
    uint32_t offset;
    Jbig2ArithCx *GB_stats = NULL;
    Jbig2ArithCx *GR_stats = NULL;
    int table_index = 0;
    const Jbig2HuffmanParams *huffman_params;

    if (segment->data_length < 10)
        goto too_short;

    /* 7.4.2.1.1 */
    flags = jbig2_get_uint16(segment_data);

    /* zero params to ease cleanup later */
    memset(&params, 0, sizeof(Jbig2SymbolDictParams));

    params.SDHUFF = flags & 1;
    params.SDREFAGG = (flags >> 1) & 1;
    params.SDTEMPLATE = (flags >> 10) & 3;
    params.SDRTEMPLATE = (flags >> 12) & 1;

    if (params.SDHUFF) {
        switch ((flags & 0x000c) >> 2) {
        case 0:                /* Table B.4 */
            params.SDHUFFDH = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_D);
            break;
        case 1:                /* Table B.5 */
            params.SDHUFFDH = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_E);
            break;
        case 3:                /* Custom table from referred segment */
            huffman_params = jbig2_find_table(ctx, segment, table_index);
            if (huffman_params == NULL) {
                return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Custom DH huffman table not found (%d)", table_index);
            }
            params.SDHUFFDH = jbig2_build_huffman_table(ctx, huffman_params);
            ++table_index;
            break;
        case 2:
        default:
            return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "symbol dictionary specified invalid huffman table");
        }
        if (params.SDHUFFDH == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate DH huffman table");
            goto cleanup;
        }

        switch ((flags & 0x0030) >> 4) {
        case 0:                /* Table B.2 */
            params.SDHUFFDW = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_B);
            break;
        case 1:                /* Table B.3 */
            params.SDHUFFDW = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_C);
            break;
        case 3:                /* Custom table from referred segment */
            huffman_params = jbig2_find_table(ctx, segment, table_index);
            if (huffman_params == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Custom DW huffman table not found (%d)", table_index);
                break;
            }
            params.SDHUFFDW = jbig2_build_huffman_table(ctx, huffman_params);
            ++table_index;
            break;
        case 2:
        default:
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "symbol dictionary specified invalid huffman table");
            goto cleanup;       /* Jump direct to cleanup to avoid 2 errors being given */
        }
        if (params.SDHUFFDW == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate DW huffman table");
            goto cleanup;
        }

        if (flags & 0x0040) {
            /* Custom table from referred segment */
            huffman_params = jbig2_find_table(ctx, segment, table_index);
            if (huffman_params == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Custom BMSIZE huffman table not found (%d)", table_index);
            } else {
                params.SDHUFFBMSIZE = jbig2_build_huffman_table(ctx, huffman_params);
                ++table_index;
            }
        } else {
            /* Table B.1 */
            params.SDHUFFBMSIZE = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_A);
        }
        if (params.SDHUFFBMSIZE == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate BMSIZE huffman table");
            goto cleanup;
        }

        if (flags & 0x0080) {
            /* Custom table from referred segment */
            huffman_params = jbig2_find_table(ctx, segment, table_index);
            if (huffman_params == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Custom REFAGG huffman table not found (%d)", table_index);
            } else {
                params.SDHUFFAGGINST = jbig2_build_huffman_table(ctx, huffman_params);
                ++table_index;
            }
        } else {
            /* Table B.1 */
            params.SDHUFFAGGINST = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_A);
        }
        if (params.SDHUFFAGGINST == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate REFAGG huffman table");
            goto cleanup;
        }
    }

    /* FIXME: there are quite a few of these conditions to check */
    /* maybe #ifdef CONFORMANCE and a separate routine */
    if (!params.SDHUFF) {
        if (flags & 0x000c) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "SDHUFF is zero, but contrary to spec SDHUFFDH is not.");
            goto cleanup;
        }
        if (flags & 0x0030) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "SDHUFF is zero, but contrary to spec SDHUFFDW is not.");
            goto cleanup;
        }
    }

    if (flags & 0x0080) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "bitmap coding context is used (NYI) symbol data likely to be garbage!");
        goto cleanup;
    }

    /* 7.4.2.1.2 */
    sdat_bytes = params.SDHUFF ? 0 : params.SDTEMPLATE == 0 ? 8 : 2;
    memcpy(params.sdat, segment_data + 2, sdat_bytes);
    offset = 2 + sdat_bytes;

    /* 7.4.2.1.3 */
    if (params.SDREFAGG && !params.SDRTEMPLATE) {
        if (offset + 4 > segment->data_length)
            goto too_short;
        memcpy(params.sdrat, segment_data + offset, 4);
        offset += 4;
    }

    if (offset + 8 > segment->data_length)
        goto too_short;

    /* 7.4.2.1.4 */
    params.SDNUMEXSYMS = jbig2_get_uint32(segment_data + offset);
    /* 7.4.2.1.5 */
    params.SDNUMNEWSYMS = jbig2_get_uint32(segment_data + offset + 4);
    offset += 8;

    jbig2_error(ctx, JBIG2_SEVERITY_INFO, segment->number,
                "symbol dictionary, flags=%04x, %u exported syms, %u new syms", flags, params.SDNUMEXSYMS, params.SDNUMNEWSYMS);

    /* 7.4.2.2 (2) */
    {
        uint32_t n_dicts = jbig2_sd_count_referred(ctx, segment);
        Jbig2SymbolDict **dicts = NULL;

        if (n_dicts > 0) {
            dicts = jbig2_sd_list_referred(ctx, segment);
            if (dicts == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate dicts in symbol dictionary");
                goto cleanup;
            }
            params.SDINSYMS = jbig2_sd_cat(ctx, n_dicts, dicts);
            if (params.SDINSYMS == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate symbol array in symbol dictionary");
                jbig2_free(ctx->allocator, dicts);
                goto cleanup;
            }
            jbig2_free(ctx->allocator, dicts);
        }
        if (params.SDINSYMS != NULL) {
            params.SDNUMINSYMS = params.SDINSYMS->n_symbols;
        }
    }

    /* 7.4.2.2 (3, 4) */
    if (flags & 0x0100) {
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "segment marks bitmap coding context as used (NYI)");
        goto cleanup;
    } else {
        int stats_size = params.SDTEMPLATE == 0 ? 65536 : params.SDTEMPLATE == 1 ? 8192 : 1024;

        GB_stats = jbig2_new(ctx, Jbig2ArithCx, stats_size);
        if (GB_stats == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to allocate GB_stats in jbig2_symbol_dictionary");
            goto cleanup;
        }
        memset(GB_stats, 0, stats_size);

        stats_size = params.SDRTEMPLATE ? 1 << 10 : 1 << 13;
        GR_stats = jbig2_new(ctx, Jbig2ArithCx, stats_size);
        if (GR_stats == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to allocate GR_stats in jbig2_symbol_dictionary");
            jbig2_free(ctx->allocator, GB_stats);
            goto cleanup;
        }
        memset(GR_stats, 0, stats_size);
    }

    segment->result = (void *)jbig2_decode_symbol_dict(ctx, segment, &params, segment_data + offset, segment->data_length - offset, GB_stats, GR_stats);
#ifdef DUMP_SYMDICT
    if (segment->result)
        jbig2_dump_symbol_dict(ctx, segment);
#endif

    /* 7.4.2.2 (7) */
    if (flags & 0x0200) {
        /* todo: retain GB_stats, GR_stats */
        jbig2_free(ctx->allocator, GR_stats);
        jbig2_free(ctx->allocator, GB_stats);
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "segment marks bitmap coding context as retained (NYI)");
    } else {
        jbig2_free(ctx->allocator, GR_stats);
        jbig2_free(ctx->allocator, GB_stats);
    }

cleanup:
    if (params.SDHUFF) {
        jbig2_release_huffman_table(ctx, params.SDHUFFDH);
        jbig2_release_huffman_table(ctx, params.SDHUFFDW);
        jbig2_release_huffman_table(ctx, params.SDHUFFBMSIZE);
        jbig2_release_huffman_table(ctx, params.SDHUFFAGGINST);
    }
    jbig2_sd_release(ctx, params.SDINSYMS);

    return (segment->result != NULL) ? 0 : -1;

too_short:
    return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Segment too short");
}