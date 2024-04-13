void CLASS parse_makernote_0xc634(int base, int uptag, unsigned dng_writer)
{
  unsigned offset = 0, entries, tag, type, len, save, c;
  unsigned i;

  uchar NikonKey, ci, cj, ck;
  unsigned serial = 0;
  unsigned NikonLensDataVersion = 0;
  unsigned lenNikonLensData = 0;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;

  unsigned uidPentax = 0;

  uchar *table_buf;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_present = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_present = 0;

  short morder, sorder = order;
  char buf[10];

  fread(buf, 1, 10, ifp);
  if (!strcmp(buf, "Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek(ifp, offset - 8, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMPUS") ||
           !strcmp(buf, "PENTAX ") ||
           (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG))) {
    base = ftell(ifp) - 10;
    fseek(ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();
  }
  else if (!strncmp(buf, "SONY", 4) ||
           !strcmp(buf, "Panasonic")) {
    goto nf;
  }
  else if (!strncmp(buf, "FUJIFILM", 8)) {
    base = ftell(ifp) - 10;
  nf: order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMP") ||
           !strcmp(buf, "LEICA") ||
           !strcmp(buf, "Ricoh") ||
           !strcmp(buf, "EPSON"))
    fseek(ifp, -2, SEEK_CUR);
  else if (!strcmp(buf, "AOC") ||
           !strcmp(buf, "QVC"))
    fseek(ifp, -4, SEEK_CUR);
  else {
    fseek(ifp, -10, SEEK_CUR);
    if ((!strncmp(make, "SAMSUNG", 7) &&
				(dng_writer == AdobeDNG)))
      base = ftell(ifp);
  }

  entries = get2();

// if (dng_writer == AdobeDNG)
//   printf("\n*** parse_makernote_0xc634: AdobeDNG");
// else if (dng_writer == CameraDNG)
//   printf("\n*** parse_makernote_0xc634: CameraDNG");

//   printf ("\n\tbuf  =%s=\n\tmake  =%s=\n\tmodel =%s=\n\tbase: 0x%x\n\tentries: %d\n",
//   		buf, make, model, base, entries);

  if (entries > 1000) return;
  morder = order;
  while (entries--) {
    order = morder;
    tiff_get(base, &tag, &type, &len, &save);
    tag |= uptag << 16;

// 	printf ("\n\tbase: 0x%x tag: 0x%04x type: 0x%x len: 0x%x pos: 0x%llx",
// 			base, tag, type, len, ftell(ifp));

    if (!strcmp(make, "Canon"))
      {
        if (tag == 0x0001)				// camera settings
          {
            fseek(ifp, 44, SEEK_CUR);
            imgdata.lens.makernotes.LensID = get2();
            imgdata.lens.makernotes.MaxFocal = get2();
            imgdata.lens.makernotes.MinFocal = get2();
            imgdata.lens.canon.CanonFocalUnits = get2();
            if (imgdata.lens.canon.CanonFocalUnits != 1)
              {
                imgdata.lens.makernotes.MaxFocal /= (float)imgdata.lens.canon.CanonFocalUnits;
                imgdata.lens.makernotes.MinFocal /= (float)imgdata.lens.canon.CanonFocalUnits;
              }
            imgdata.lens.makernotes.MaxAp = powf64(2.0f, _CanonConvert2EV(get2()) / 2.0f);
            imgdata.lens.makernotes.MinAp = powf64(2.0f, _CanonConvert2EV(get2()) / 2.0f);
          }

        else if (tag == 0x0002)			// focal length
          {
            imgdata.lens.makernotes.FocalType = get2();
            imgdata.lens.makernotes.CurFocal = get2();
            if ((imgdata.lens.canon.CanonFocalUnits != 1) &&
                imgdata.lens.canon.CanonFocalUnits)
              {
                imgdata.lens.makernotes.CurFocal /= (float)imgdata.lens.canon.CanonFocalUnits;
              }
          }

        else if (tag == 0x0004)			// shot info
          {
            fseek(ifp, 42, SEEK_CUR);
            imgdata.lens.makernotes.CurAp = powf64(2.0f, _CanonConvert2EV(get2()) / 2.0f);
          }

        else if (tag == 0x000d)			// camera info
          {
            CanonCameraInfo = (uchar*)malloc(len);
            fread(CanonCameraInfo, len, 1, ifp);
            lenCanonCameraInfo = len;
          }

        else if (tag == 0x10 && type == 4)	// Canon ModelID
          {
            unique_id = get4();
            setCanonBodyFeatures(unique_id);
            if (lenCanonCameraInfo) processCanonCameraInfo(unique_id, CanonCameraInfo);
          }

        else if (tag == 0x0095 &&		// lens model tag
                 !imgdata.lens.makernotes.Lens[0])
          {
            fread(imgdata.lens.makernotes.Lens, 2, 1, ifp);
            imgdata.lens.makernotes.LensMount = Canon_EF;
            if (imgdata.lens.makernotes.Lens[0] < 65)					// non-Canon lens
              fread(imgdata.lens.makernotes.Lens + 2, 62, 1, ifp);
            else
              {
                char efs[2];
                imgdata.lens.makernotes.LensFeatures_pre[0] = imgdata.lens.makernotes.Lens[0];
                imgdata.lens.makernotes.LensFeatures_pre[1] = imgdata.lens.makernotes.Lens[1];
                fread(efs, 2, 1, ifp);
                if (efs[0] == 45 && (efs[1] == 83 || efs[1] == 69 || efs[1] == 77))
                  {	// "EF-S, TS-E, MP-E, EF-M" lenses
                    imgdata.lens.makernotes.Lens[2] = imgdata.lens.makernotes.LensFeatures_pre[2] = efs[0];
                    imgdata.lens.makernotes.Lens[3] = imgdata.lens.makernotes.LensFeatures_pre[3] = efs[1];
                    imgdata.lens.makernotes.Lens[4] = 32;
                    if (efs[1] == 83)
                      {
                        imgdata.lens.makernotes.LensMount = Canon_EF_S;
                        imgdata.lens.makernotes.LensFormat = APSC;
                      }
                    else if (efs[1] == 77)
                      {
                        imgdata.lens.makernotes.LensMount = Canon_EF_M;
                      }
                  }
                else
                  {																// "EF" lenses
                    imgdata.lens.makernotes.Lens[2] = 32;
                    imgdata.lens.makernotes.Lens[3] = efs[0];
                    imgdata.lens.makernotes.Lens[4] = efs[1];
                  }
                fread(imgdata.lens.makernotes.Lens + 5, 58, 1, ifp);
              }
          }

        else if (tag == 0x4021)			// multi-exposure tag
          {
            get4();
            if (get4() == 2)
              {
                imgdata.color.wb_applied = 1;
              }
          }
      }

    else if (!strncmp(make, "FUJI", 4))
      switch (tag) {
      case 0x1404: imgdata.lens.makernotes.MinFocal = getreal(type); break;
      case 0x1405: imgdata.lens.makernotes.MaxFocal = getreal(type); break;
      case 0x1406: imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type); break;
      case 0x1407: imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type); break;
      }

    else if (!strncmp(make, "NIKON", 5))
      {
        if (tag == 0x1d)							// serial number
          while ((c = fgetc(ifp)) && c != EOF)
            serial = serial * 10 + (isdigit(c) ? c - '0' : c % 10);

        else if (tag == 0x0082)				// lens attachment
          {
            fread(imgdata.lens.makernotes.Attachment, len, 1, ifp);
          }
        else if (tag == 0x0083)				// lens type
          {
            imgdata.lens.nikon.NikonLensType = fgetc(ifp);
            if (!(imgdata.lens.nikon.NikonLensType & 0x01))
              {
                imgdata.lens.makernotes.LensFeatures_pre[0] = 'A';
                imgdata.lens.makernotes.LensFeatures_pre[1] = 'F';
              }
            if (imgdata.lens.nikon.NikonLensType & 0x02)
              {
                if (imgdata.lens.nikon.NikonLensType & 0x04)
                  imgdata.lens.makernotes.LensFeatures_suf[0] = 'G';
                else
                  imgdata.lens.makernotes.LensFeatures_suf[0] = 'D';
                imgdata.lens.makernotes.LensFeatures_suf[1] = ' ';
              }
            if (imgdata.lens.nikon.NikonLensType & 0x08)
              {
                imgdata.lens.makernotes.LensFeatures_suf[2] = 'V';
                imgdata.lens.makernotes.LensFeatures_suf[3] = 'R';
              }
            if (imgdata.lens.nikon.NikonLensType & 0x10)
              {
                imgdata.lens.makernotes.LensMount = Nikon_CX;
              }

            if (imgdata.lens.nikon.NikonLensType & 0x20)
              {
                strcpy(imgdata.lens.makernotes.Adapter, "FT-1");
              }
            imgdata.lens.nikon.NikonLensType = imgdata.lens.nikon.NikonLensType & 0xdf;
          }
        else if (tag == 0x0084)				// lens
          {
            imgdata.lens.makernotes.MinFocal = getreal(type);
            imgdata.lens.makernotes.MaxFocal = getreal(type);
            imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type);
            imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type);
          }
        else if (tag == 0x008b)				// lens f-stops
          {
            uchar a, b, c;
            a = fgetc(ifp);
            b = fgetc(ifp);
            c = fgetc(ifp);
            if (c)
              {
                imgdata.lens.nikon.NikonLensFStops = a*b*(12/c);
                imgdata.lens.makernotes.LensFStops =
                  (float)imgdata.lens.nikon.NikonLensFStops /12.0f;
              }
          }
        else if ((tag == 0x0093) &&		// compression
                 (get2() == 8))				// sRAW
          {
            imgdata.color.wb_applied = 1;
            imgdata.color.gamma_applied = 1;
          }

        else if (tag == 0x0098)				// contains lens data
          {
            for (i = 0; i < 4; i++)
              {
                NikonLensDataVersion = NikonLensDataVersion * 10 + fgetc(ifp) - '0';
              }
            switch (NikonLensDataVersion)
              {
              case 100: lenNikonLensData = 9; imgdata.lens.makernotes.LensMount = Nikon_F; break;
              case 101:
              case 201:	// encrypted, starting from v.201
              case 202:
              case 203: lenNikonLensData = 15; imgdata.lens.makernotes.LensMount = Nikon_F; break;
              case 204: lenNikonLensData = 16; imgdata.lens.makernotes.LensMount = Nikon_F; break;
              case 400: lenNikonLensData = 459; break;
              case 401: lenNikonLensData = 590; break;
              case 402: lenNikonLensData = 509; break;
              case 403: lenNikonLensData = 879; break;
              }
            table_buf = (uchar*)malloc(lenNikonLensData);
            fread(table_buf, lenNikonLensData, 1, ifp);
            if ((NikonLensDataVersion < 201) && lenNikonLensData)
            {
              processNikonLensData(table_buf, lenNikonLensData);
              lenNikonLensData = 0;
            }
          }

        else if (tag == 0xa7)					// shutter count
          {
            NikonKey = fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp);
            if ((NikonLensDataVersion > 200) && lenNikonLensData)
              {
                ci = xlat[0][serial & 0xff];
                cj = xlat[1][NikonKey];
                ck = 0x60;
                for (i = 0; i < lenNikonLensData; i++)
                  table_buf[i] ^= (cj += ci * ck++);
                processNikonLensData(table_buf, lenNikonLensData);
                lenNikonLensData = 0;
              }
          }

        else if (tag == 37 && (!iso_speed || iso_speed == 65535))
          {
            unsigned char cc;
            fread(&cc, 1, 1, ifp);
            iso_speed = (int)(100.0 * powf64(2.0, (double)(cc) / 12.0 - 5.0));
            break;
          }
      }

    else if (!strncmp(make, "OLYMPUS", 7))
      {
        if (tag == 0x2010)
          {
            fseek(ifp, save - 4, SEEK_SET);
            fseek(ifp, base + get4(), SEEK_SET);
            parse_makernote_0xc634(base, 0x2010, dng_writer);
          }

        switch (tag) {
        case 0x0207:
        case 0x20100100:
          {
            uchar sOlyID[7];
            long unsigned OlyID;
            fread (sOlyID, len, 1, ifp);
            OlyID = sOlyID[0];
            i = 1;
            while (sOlyID[i])
              {
                OlyID = OlyID << 8 | sOlyID[i];
                i++;
              }
            setOlympusBodyFeatures(OlyID);
          }
          break;
        case 0x1002:
          imgdata.lens.makernotes.CurAp = powf64(2.0f, getreal(type)/2);
          break;
        case 0x20100201:
          imgdata.lens.makernotes.LensID =
            (unsigned long long)fgetc(ifp)<<16 |
            (unsigned long long)(fgetc(ifp), fgetc(ifp))<<8 |
            (unsigned long long)fgetc(ifp);
          imgdata.lens.makernotes.LensMount = FT;
          imgdata.lens.makernotes.LensFormat = FT;
          if (((imgdata.lens.makernotes.LensID < 0x20000) ||
               (imgdata.lens.makernotes.LensID > 0x4ffff)) &&
              (imgdata.lens.makernotes.LensID & 0x10))
            {
              imgdata.lens.makernotes.LensMount = mFT;
            }
          break;
        case 0x20100203:
          fread(imgdata.lens.makernotes.Lens, len, 1, ifp);
          break;
        case 0x20100205:
          imgdata.lens.makernotes.MaxAp4MinFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100206:
          imgdata.lens.makernotes.MaxAp4MaxFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100207:
          imgdata.lens.makernotes.MinFocal = (float)get2();
          break;
        case 0x20100208:
          imgdata.lens.makernotes.MaxFocal = (float)get2();
          if (imgdata.lens.makernotes.MaxFocal > 1000.0f)
            imgdata.lens.makernotes.MaxFocal = imgdata.lens.makernotes.MinFocal;
          break;
        case 0x2010020a:
          imgdata.lens.makernotes.MaxAp4CurFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100301:
          imgdata.lens.makernotes.TeleconverterID = fgetc(ifp) << 8;
          fgetc(ifp);
          imgdata.lens.makernotes.TeleconverterID =
            imgdata.lens.makernotes.TeleconverterID | fgetc(ifp);
          break;
        case 0x20100303:
          fread(imgdata.lens.makernotes.Teleconverter, len, 1, ifp);
          break;
        case 0x20100403:
          fread(imgdata.lens.makernotes.Attachment, len, 1, ifp);
          break;
        }
      }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(model, "PENTAX", 6) ||
             (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG)))
      {
        if (tag == 0x0005)
          {
            uidPentax = unique_id = get4();
            setPentaxBodyFeatures(unique_id);
            if (
                (dng_writer == CameraDNG) &&
                (
                 (uidPentax == 0x12f66) ||		// Q10
                 (uidPentax == 0x12f7a) ||		// Q7
                 (uidPentax == 0x12ee4)			// Q
                 )
                )
              base += 10;
          }
        else if (tag == 0x0013)
          {
            imgdata.lens.makernotes.CurAp = (float)get2()/10.0f;
          }
        else if (tag == 0x001d)
          {
            imgdata.lens.makernotes.CurFocal = (float)get4()/100.0f;
          }
        else if (tag == 0x003f)
          {
            imgdata.lens.makernotes.LensID = fgetc(ifp) << 8 | fgetc(ifp);
          }
        else if (tag == 0x0207)
          {
            ushort iLensData = 0;
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            if ((uidPentax < 0x12b9c) ||
                ((uidPentax == 0x12b9c) ||	// K100D
                 (uidPentax == 0x12b9d) ||	// K110D
                 (uidPentax == 0x12ba2)	&&	// K100D Super
                 (!table_buf[20] || (table_buf[20] == 0xff))))
              {
                iLensData = 3;
                if (imgdata.lens.makernotes.LensID == -1)
                  imgdata.lens.makernotes.LensID =
                    (((unsigned)table_buf[0]) << 8) + table_buf[1];
              }
            else switch (len)
              {
              case 90:							// LensInfo3
                iLensData = 13;
                if (imgdata.lens.makernotes.LensID == -1)
                  imgdata.lens.makernotes.LensID =
                    ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) <<8) + table_buf[4];
                break;
              case 91:							// LensInfo4
                iLensData = 12;
                if (imgdata.lens.makernotes.LensID == -1)
                  imgdata.lens.makernotes.LensID =
                    ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) <<8) + table_buf[4];
                break;
              case 80:							// LensInfo5
              case 128:
                iLensData = 15;
                if (imgdata.lens.makernotes.LensID == -1)
                  imgdata.lens.makernotes.LensID =
                    ((unsigned)((table_buf[1] & 0x0f) + table_buf[4]) <<8) + table_buf[5];
                break;
              default:
                if (uidPentax >= 0x12b9c)		// LensInfo2
                  {
                    iLensData = 4;
                    if (imgdata.lens.makernotes.LensID == -1)
                      imgdata.lens.makernotes.LensID =
                        ((unsigned)((table_buf[0] & 0x0f) + table_buf[2]) <<8) + table_buf[3];
                  }
              }
            if (iLensData)
              {
                if (table_buf[iLensData+9] &&
                    (fabs(imgdata.lens.makernotes.CurFocal) < 0.1f))
                  imgdata.lens.makernotes.CurFocal =
                    10*(table_buf[iLensData+9]>>2) * powf64(4, (table_buf[iLensData+9] & 0x03)-2);
                if (table_buf[iLensData+10] & 0xf0)
                  imgdata.lens.makernotes.MaxAp4CurFocal =
                    powf64(2.0f, (float)((table_buf[iLensData+10] & 0xf0) >>4)/4.0f);
                if (table_buf[iLensData+10] & 0x0f)
                  imgdata.lens.makernotes.MinAp4CurFocal =
                    powf64(2.0f, (float)((table_buf[iLensData+10] & 0x0f) + 10)/4.0f);
                if (
                    (uidPentax != 0x12e6c) &&	// K-r
                    (uidPentax != 0x12e76) &&	// K-5
                    (uidPentax != 0x12f70)		// K-5 II
                    //        	  		(uidPentax != 0x12f71)		// K-5 II s
                    )
                  {
                    switch (table_buf[iLensData] & 0x06)
                      {
                      case 0: imgdata.lens.makernotes.MinAp4MinFocal = 22.0f; break;
                      case 2: imgdata.lens.makernotes.MinAp4MinFocal = 32.0f; break;
                      case 4: imgdata.lens.makernotes.MinAp4MinFocal = 45.0f; break;
                      case 6: imgdata.lens.makernotes.MinAp4MinFocal = 16.0f; break;
                      }
                    if (table_buf[iLensData] & 0x70)
                      imgdata.lens.makernotes.LensFStops =
                        ((float)(((table_buf[iLensData] & 0x70) >> 4) ^ 0x07)) / 2.0f + 5.0f;
                    if ((table_buf[iLensData+14] > 1) &&
                        (fabs(imgdata.lens.makernotes.MaxAp4CurFocal) < 0.7f))
                      imgdata.lens.makernotes.MaxAp4CurFocal =
                        powf64(2.0f, (float)((table_buf[iLensData+14] & 0x7f) -1)/32.0f);
                  }
                else if ((uidPentax != 0x12e76) &&	// K-5
                         (table_buf[iLensData+15] > 1) &&
                         (fabs(imgdata.lens.makernotes.MaxAp4CurFocal) < 0.7f))
                  {
                    imgdata.lens.makernotes.MaxAp4CurFocal =
                      powf64(2.0f, (float)((table_buf[iLensData+15] & 0x7f) -1)/32.0f);
                  }
              }
            free(table_buf);
          }
        else if (tag == 0x0239)		// Q-series lens info (LensInfoQ)
          {
            char LensInfo [20];
            fseek (ifp, 2, SEEK_CUR);
            fread(imgdata.lens.makernotes.Lens, 30, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, " ");
            fread(LensInfo, 20, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, LensInfo);
          }
      }

    else if (!strncmp(make, "SAMSUNG", 7) &&
             (dng_writer == AdobeDNG))
      {
        if (tag == 0x0002)
          {
            if(get4() == 0x2000)
              {
                imgdata.lens.makernotes.CameraMount = Samsung_NX;
              }
            else
              {
                imgdata.lens.makernotes.CameraMount = FixedLens;
                imgdata.lens.makernotes.LensMount = FixedLens;
              }
          }
        else if (tag == 0x0003)
          {
            unique_id = get4();
          }
        else if (tag == 0xa003)
          {
            imgdata.lens.makernotes.LensID = get2();
            if (imgdata.lens.makernotes.LensID)
              imgdata.lens.makernotes.LensMount = Samsung_NX;
          }
        else if (tag == 0xa019)
          {
            imgdata.lens.makernotes.CurAp = getreal(type);
          }
        else if (tag == 0xa01a)
          {
            imgdata.lens.samsung.FocalLengthIn35mmFormat = get4() / 10.0f;
            if (imgdata.lens.samsung.FocalLengthIn35mmFormat < 10.0f)
              imgdata.lens.samsung.FocalLengthIn35mmFormat *= 10.0f;
          }
      }

    else if (!strncasecmp(make, "SONY", 4) ||
             !strncasecmp(make, "Konica", 6) ||
             !strncasecmp(make, "Minolta", 7))
      {
        ushort lid;

        if (tag == 0xb001)			// Sony ModelID
          {
            unique_id = imgdata.lens.makernotes.CamID = get2();
            setSonyBodyFeatures(unique_id);
            if (table_buf_0x9050_present)
              {
                process_Sony_0x9050(table_buf_0x9050, unique_id);
                free (table_buf_0x9050);
                table_buf_0x9050_present = 0;
              }
            if (table_buf_0x940c_present)
              {
                if (imgdata.lens.makernotes.CameraMount == Sony_E)
                  {
                    process_Sony_0x940c(table_buf_0x940c);
                  }
                free (table_buf_0x940c);
                table_buf_0x940c_present = 0;
              }
          }
        else if ((tag == 0x0010) &&					// CameraInfo
                 strncasecmp(model, "DSLR-A100", 9) &&
                 strncasecmp(model, "NEX-5C", 6) &&
                 !strncasecmp(make, "SONY", 4) &&
                 ((len == 368) ||			// a700
                  (len == 5478) ||		// a850, a900
                  (len == 5506) ||		// a200, a300, a350
                  (len == 6118) ||		// a230, a290, a330, a380, a390

                  // a450, a500, a550, a560, a580
                  // a33, a35, a55
                  // NEX3, NEX5, NEX5C, NEXC3, VG10E
                  (len == 15360))
                 )
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            if (memcmp(table_buf, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) &&
                memcmp(table_buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
              {
                switch (len) {
                case 368:
                case 5478:
                  // a700, a850, a900: CameraInfo
                  if (saneSonyCameraInfo(table_buf[0], table_buf[3], table_buf[2], table_buf[5], table_buf[4], table_buf[7]))
                    {
                      if (table_buf[0] | table_buf[3])
                        imgdata.lens.makernotes.MinFocal =
                          bcd2dec(table_buf[0]) * 100 + bcd2dec(table_buf[3]);
                      if (table_buf[2] | table_buf[5])
                        imgdata.lens.makernotes.MaxFocal =
                          bcd2dec(table_buf[2]) * 100 + bcd2dec(table_buf[5]);
                      if (table_buf[4])
                        imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[4]) / 10.0f;
                      if (table_buf[4])
                        imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[7]) / 10.0f;
                      parseSonyLensFeatures(table_buf[1], table_buf[6]);
                    }
                  break;
                default:
                  // CameraInfo2 & 3
                  if (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6]))
                    {
                      if (table_buf[1] | table_buf[2])
                        imgdata.lens.makernotes.MinFocal =
                          bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
                      if (table_buf[3] | table_buf[4])
                        imgdata.lens.makernotes.MaxFocal =
                          bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
                      if (table_buf[5])
                        imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
                      if (table_buf[6])
                        imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
                      parseSonyLensFeatures(table_buf[0], table_buf[7]);
                    }
                }
              }
            free(table_buf);
          }

        else if (tag == 0x0105)					// Teleconverter
          {
            imgdata.lens.makernotes.TeleconverterID = get2();
          }

        else if (tag == 0x0114)					// CameraSettings
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            switch (len) {
            case 280:
            case 364:
            case 332:
              // CameraSettings and CameraSettings2 are big endian
              if (table_buf[2] | table_buf[3])
                {
                  lid = (((ushort)table_buf[2])<<8) |
                    ((ushort)table_buf[3]);
                  imgdata.lens.makernotes.CurAp =
                    powf64(2.0f, ((float)lid/8.0f-1.0f)/2.0f);
                }
              break;
            case 1536:
            case 2048:
              // CameraSettings3 are little endian
              parseSonyLensType2(table_buf[1016], table_buf[1015]);
              if (imgdata.lens.makernotes.LensMount != Canon_EF)
                {
                  switch (table_buf[153]) {
                  case 16: imgdata.lens.makernotes.LensMount = Minolta_A; break;
                  case 17: imgdata.lens.makernotes.LensMount = Sony_E; break;
                  }
                }
              break;
            }
            free(table_buf);
          }

        else if (tag == 0x9050)		// little endian
          {
            table_buf_0x9050 = (uchar*)malloc(len);
            table_buf_0x9050_present = 1;
            fread(table_buf_0x9050, len, 1, ifp);

            if (imgdata.lens.makernotes.CamID)
              {
                process_Sony_0x9050(table_buf_0x9050, imgdata.lens.makernotes.CamID);
                free (table_buf_0x9050);
                table_buf_0x9050_present = 0;
              }
          }

        else if (tag == 0x940c)
          {
            table_buf_0x940c = (uchar*)malloc(len);
            table_buf_0x940c_present = 1;
            fread(table_buf_0x940c, len, 1, ifp);
            if ((imgdata.lens.makernotes.CamID) &&
                (imgdata.lens.makernotes.CameraMount == Sony_E))
              {
                process_Sony_0x940c(table_buf_0x940c);
                free(table_buf_0x940c);
                table_buf_0x940c_present = 0;
              }
          }

        else if (((tag == 0xb027) || (tag == 0x010c)) && (imgdata.lens.makernotes.LensID == -1))
          {
            imgdata.lens.makernotes.LensID = get4();
            if ((imgdata.lens.makernotes.LensID > 61184) &&
                (imgdata.lens.makernotes.LensID < 65535))
              {
                imgdata.lens.makernotes.LensID -= 61184;
                imgdata.lens.makernotes.LensMount = Canon_EF;
              }
            if (tag == 0x010c) imgdata.lens.makernotes.CameraMount = Minolta_A;
          }

        else if (tag == 0xb02a)					// Sony LensSpec
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            if (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6]))
              {
                if (table_buf[1] | table_buf[2])
                  imgdata.lens.makernotes.MinFocal =
                    bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
                if (table_buf[3] | table_buf[4])
                  imgdata.lens.makernotes.MaxFocal =
                    bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
                if (table_buf[5])
                  imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
                if (table_buf[6])
                  imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
                parseSonyLensFeatures(table_buf[0], table_buf[7]);
              }
            free(table_buf);
          }
      }
  next:
    fseek (ifp, save, SEEK_SET);
  }
 quit:
  order = sorder;
}