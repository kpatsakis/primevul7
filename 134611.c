void CLASS parse_makernote (int base, int uptag)
{
  unsigned offset=0, entries, tag, type, len, save, c;
  unsigned ver97=0, serial=0, i, wbi=0, wb[4]={0,0,0,0};
  uchar buf97[324], ci, cj, ck;
  short morder, sorder=order;
  char buf[10];
  unsigned SamsungKey[11];
  static const double rgb_adobe[3][3] =		// inv(sRGB2XYZ_D65) * AdobeRGB2XYZ_D65
    {{ 1.398283396477404,     -0.398283116703571, 4.427165001263944E-08},
     {-1.233904514232401E-07,  0.999999995196570, 3.126724276714121e-08},
     { 4.561487232726535E-08, -0.042938290466635, 1.042938250416105    }};

  float adobe_cam [3][3];
  uchar NikonKey;

#ifdef LIBRAW_LIBRARY_BUILD
  unsigned NikonLensDataVersion = 0;
  unsigned lenNikonLensData = 0;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;

  unsigned uidPentax = 0;
  unsigned long long tLensID;

  uchar *table_buf;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_present = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_present = 0;
#endif
/*
   The MakerNote might have its own TIFF header (possibly with
   its own byte-order!), or it might just be a table.
 */
  if (!strcmp(make,"Nokia")) return;
  fread (buf, 1, 10, ifp);
  if (!strncmp (buf,"KDK" ,3) ||	/* these aren't TIFF tables */
      !strncmp (buf,"VER" ,3) ||
      !strncmp (buf,"IIII",4) ||
      !strncmp (buf,"MMMM",4)) return;
  if (!strncmp (buf,"KC"  ,2) ||	/* Konica KD-400Z, KD-510Z */
      !strncmp (buf,"MLY" ,3)) {	/* Minolta DiMAGE G series */
    order = 0x4d4d;
    while ((i=ftell(ifp)) < data_offset && i < 16384) {
      wb[0] = wb[2];  wb[2] = wb[1];  wb[1] = wb[3];
      wb[3] = get2();
      if (wb[1] == 256 && wb[3] == 256 &&
	  wb[0] > 256 && wb[0] < 640 && wb[2] > 256 && wb[2] < 640)
	FORC4 cam_mul[c] = wb[c];
    }
    goto quit;
  }
  if (!strcmp (buf,"Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek (ifp, offset-8, SEEK_CUR);
  } else if (!strcmp (buf,"OLYMPUS") ||
             !strcmp (buf,"PENTAX ")) {
    base = ftell(ifp)-10;
    fseek (ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();
  } else if (!strncmp (buf,"SONY",4) ||
						 !strcmp  (buf,"Panasonic")) {
    goto nf;
  } else if (!strncmp (buf,"FUJIFILM",8)) {
    base = ftell(ifp)-10;
	nf: order = 0x4949;
    fseek (ifp,  2, SEEK_CUR);
  } else if (!strcmp (buf,"OLYMP") ||
						 !strcmp (buf,"LEICA") ||
						 !strcmp (buf,"Ricoh") ||
						 !strcmp (buf,"EPSON"))
    fseek (ifp, -2, SEEK_CUR);
  else if (!strcmp (buf,"AOC") ||
					 !strcmp (buf,"QVC"))
    fseek (ifp, -4, SEEK_CUR);
  else {
    fseek (ifp, -10, SEEK_CUR);
    if (!strncmp(make,"SAMSUNG",7))
      base = ftell(ifp);
  }

  // adjust pos & base for Leica M8/M9/M Mono tags and dir in tag 0x3400
  if (!strncasecmp(make, "LEICA", 5))
    {
      if (!strncmp(model, "M8", 2))
        {
          base = ftell(ifp)-8;
        }
      else if (!strncmp(model, "M9", 2) ||
               !strncmp(model, "M Monochrom", 11))
        {
          if (!uptag)
            {
              base = ftell(ifp) - 10;
              fseek (ifp, 8, SEEK_CUR);
            }
          else if (uptag == 0x3400)
            {
              fseek (ifp, 10, SEEK_CUR);
              base += 10;
            }
        }
    }

  entries = get2();

//   printf("\n*** parse_makernote\n\tmake  =%s=\n\tmodel =%s= \n\tentries: %d\n",
// 		make, model, entries);

  if (entries > 1000) return;
  morder = order;
  while (entries--) {
    order = morder;
    tiff_get (base, &tag, &type, &len, &save);
    tag |= uptag << 16;

// 	printf ("\n\tbase: 0x%x tag: 0x%04x type: 0x%x len: 0x%x pos: 0x%llx",
// 		base, tag, type, len, ftell(ifp));

#ifdef LIBRAW_LIBRARY_BUILD

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

    else if (!strncasecmp(make, "LEICA", 5))
      {
        if ((tag == 0x0303) && (type != 4))
          {
            fread(imgdata.lens.makernotes.Lens, len, 1, ifp);
          }

        if ((tag == 0x3405) ||
            (tag == 0x0310) ||
            (tag == 0x34003405))
          {
            imgdata.lens.makernotes.LensID = get4();
            imgdata.lens.makernotes.LensID =
              ((imgdata.lens.makernotes.LensID>>2)<<8) |
              (imgdata.lens.makernotes.LensID & 0x3);
            if (imgdata.lens.makernotes.LensID != -1)
              {
                if ((model[0] == 'M') ||
                    !strncasecmp (model, "LEICA M", 7))
                  {
                    imgdata.lens.makernotes.LensMount = Leica_M;
                  }
                else if ((model[0] == 'S') ||
                         !strncasecmp (model, "LEICA S", 7))
                  {
                    imgdata.lens.makernotes.LensMount = Leica_S;
                  }
              }
          }

        else if (
                 ((tag == 0x0313) || (tag == 0x34003406)) &&
                 (fabs(imgdata.lens.makernotes.CurAp) < 0.5f)
                 )
          {
            imgdata.lens.makernotes.CurAp = getreal(5);
          }

        else if (tag == 0x3400)
          {
            parse_makernote (base, 0x3400);
          }
      }

    else if (!strncmp(make, "NIKON",5))
      {
        if (tag == 0x0082)						// lens attachment
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
      }

    else if (!strncmp(make, "OLYMPUS", 7))
      {
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

    else if (!strncmp(make, "PENTAX", 6) &&
             !strncmp(model, "GR", 2))
      {
        if ((tag == 0x1001) && (type == 3))
          {
            imgdata.lens.makernotes.CameraMount = FixedLens;
            imgdata.lens.makernotes.LensMount = FixedLens;
            imgdata.lens.makernotes.CameraFormat = APSC;
            imgdata.lens.makernotes.LensID = -1;
            imgdata.lens.makernotes.FocalType = 1;
          }
        else if ((tag == 0x1017) && (get2() == 2))
          {
            strcpy(imgdata.lens.makernotes.Attachment, "Wide-Angle Adapter");
          }
        else if (tag == 0x1500)
          {
            imgdata.lens.makernotes.CurFocal = getreal(type);
          }
      }

    else if (!strncmp(make, "RICOH", 5) &&
             strncmp(model, "PENTAX", 6))
      {
        if ((tag == 0x1017) && (get2() == 2))
          {
            strcpy(imgdata.lens.makernotes.Attachment, "Wide-Angle Adapter");
          }

        else if (tag == 0x1500)
          {
            imgdata.lens.makernotes.CurFocal = getreal(type);
          }

        else if (tag == 0x2001)
          {
            short ntags, cur_tag;
            fseek(ifp, 20, SEEK_CUR);
            ntags = get2();
            cur_tag = get2();
            while (cur_tag != 0x002c)
              {
                fseek(ifp, 10, SEEK_CUR);
                cur_tag = get2();
              }
            fseek(ifp, 6, SEEK_CUR);
            fseek(ifp, get4()+34, SEEK_SET);
            imgdata.lens.makernotes.LensID = getc(ifp) - '0';
            switch(imgdata.lens.makernotes.LensID) 
              {
            	case 1:
            	case 2:
            	case 3:
            	case 5:
            	case 6:
            		imgdata.lens.makernotes.CameraMount = FixedLens;
                imgdata.lens.makernotes.LensMount = RicohModule;
                break;
              case 8:
                imgdata.lens.makernotes.CameraMount = Leica_M;
                imgdata.lens.makernotes.CameraFormat = APSC;
                imgdata.lens.makernotes.LensID = -1;
                break;
              default:
              	imgdata.lens.makernotes.LensID = -1;
              }
          }
      }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(model, "PENTAX", 6) ||
             (!strncmp(make, "SAMSUNG", 7) && dng_version) &&
             strncmp(model, "GR", 2))
      {
        if (tag == 0x0005)
          {
            uidPentax = unique_id = get4();
            setPentaxBodyFeatures(unique_id);
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
                if (table_buf[iLensData+9] && (fabs(imgdata.lens.makernotes.CurFocal) < 0.1f))
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

    else if (!strncmp(make, "SAMSUNG", 7))
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
                switch (len) 
                  {
                  case 368:
                  case 5478:
                    // a700, a850, a900: CameraInfo
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
                    break;
                  default:
                    // CameraInfo2 & 3
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
            free(table_buf);
          }
      }
#endif

    if (tag == 2 && strstr(make,"NIKON") && !iso_speed)
      iso_speed = (get2(),get2());
    if (tag == 37 && strstr(make,"NIKON") && (!iso_speed || iso_speed == 65535))
      {
        unsigned char cc;
        fread(&cc,1,1,ifp);
        iso_speed = int(100.0 * powf64(2.0f,float(cc)/12.0-5.0));
      }
    if (tag == 4 && len > 26 && len < 35) {
      if ((i=(get4(),get2())) != 0x7fff && (!iso_speed || iso_speed == 65535))
	iso_speed = 50 * powf64(2.0, i/32.0 - 4);
      if ((i=(get2(),get2())) != 0x7fff && !aperture)
	aperture = powf64(2.0, i/64.0);
      if ((i=get2()) != 0xffff && !shutter)
	shutter = powf64(2.0, (short) i/-32.0);
      wbi = (get2(),get2());
      shot_order = (get2(),get2());
    }
    if ((tag == 4 || tag == 0x114) && !strncmp(make,"KONICA",6)) {
      fseek (ifp, tag == 4 ? 140:160, SEEK_CUR);
      switch (get2()) {
	case 72:  flip = 0;  break;
	case 76:  flip = 6;  break;
	case 82:  flip = 5;  break;
      }
    }
    if (tag == 7 && type == 2 && len > 20)
      fgets (model2, 64, ifp);
    if (tag == 8 && type == 4)
      shot_order = get4();
    if (tag == 9 && !strcmp(make,"Canon"))
      fread (artist, 64, 1, ifp);
    if (tag == 0xc && len == 4)
      FORC3 cam_mul[(c << 1 | c >> 1) & 3] = getreal(type);
    if (tag == 0xd && type == 7 && get2() == 0xaaaa) {
      for (c=i=2; (ushort) c != 0xbbbb && i < len; i++)
	c = c << 8 | fgetc(ifp);
      while ((i+=4) < len-5)
	if (get4() == 257 && (i=len) && (c = (get4(),fgetc(ifp))) < 3)
	  flip = "065"[c]-'0';
    }

    if (tag == 0x10 && type == 4)
      {
        unique_id = get4();

#ifdef LIBRAW_LIBRARY_BUILD
        setCanonBodyFeatures(unique_id);
        if (lenCanonCameraInfo) processCanonCameraInfo(unique_id, CanonCameraInfo);
#endif
      }

#ifdef LIBRAW_LIBRARY_BUILD
    if(tag == 0x20400805 && len == 2 && !strncasecmp(make,"Olympus",7))
      {
        imgdata.color.OlympusSensorCalibration[0]=getreal(type);
        imgdata.color.OlympusSensorCalibration[1]=getreal(type);
      }
    if (tag == 0x4001 && len > 500 && !strcasecmp(make,"Canon"))
      {
        long int save1 = ftell(ifp);
        switch (len) 
          {
          case 582:
            imgdata.color.canon_makernotes.CanonColorDataVer = 1;	// 20D / 350D
            break;
          case 653:
            imgdata.color.canon_makernotes.CanonColorDataVer = 2;	// 1Dmk2 / 1DsMK2
            break;
          case 796:
            imgdata.color.canon_makernotes.CanonColorDataVer = 3;	// 1DmkIIN / 5D / 30D / 400D

            // 1DmkIII / 1DSmkIII / 1DmkIV / 5DmkII
            // 7D / 40D / 50D / 60D / 450D / 500D
            // 550D / 1000D / 1100D
          case 674: case 692: case 702: case 1227: case 1250:
          case 1251: case 1337: case 1338: case 1346:
            imgdata.color.canon_makernotes.CanonColorDataVer = 4;
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x0e7<<1), SEEK_SET); // offset 231 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            if ((imgdata.color.canon_makernotes.CanonColorDataSubVer == 4)
                || (imgdata.color.canon_makernotes.CanonColorDataSubVer == 5))
              {
                fseek (ifp, save1+(0x2b9<<1), SEEK_SET);		// offset 697 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            else if ((imgdata.color.canon_makernotes.CanonColorDataSubVer == 6) ||
                     (imgdata.color.canon_makernotes.CanonColorDataSubVer == 7))
              {
                fseek (ifp, save1+(0x2d0<<1), SEEK_SET);		// offset 720 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            else if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 9)
              {
                fseek (ifp, save1+(0x2d4<<1), SEEK_SET);		// offset 724 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            break;

          case 5120:
            imgdata.color.canon_makernotes.CanonColorDataVer = 5;	// PowerSot G10
            break;

          case 1273: case 1275:
            imgdata.color.canon_makernotes.CanonColorDataVer = 6;	// 600D / 1200D
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x0fb<<1), SEEK_SET);			// offset 251 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            fseek (ifp, save1+(0x1e4<<1), SEEK_SET);			// offset 484 shorts
            imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
            break;

            // 1DX / 5DmkIII / 6D / 100D / 650D / 700D / M / 7DmkII / 750D / 760D
          case 1312: case 1313: case 1316: case 1506:
            imgdata.color.canon_makernotes.CanonColorDataVer = 7;
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x114<<1), SEEK_SET);			// offset 276 shorts
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 10)
              {
                fseek (ifp, save1+(0x1fd<<1), SEEK_SET);		// offset 509 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              } else if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 11)
              {
                fseek (ifp, save1+(0x2dd<<1), SEEK_SET);		// offset 733 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            break;
          }
        fseek (ifp, save1, SEEK_SET);
      }
#endif
    if (tag == 0x11 && is_raw && !strncmp(make,"NIKON",5)) {
      fseek (ifp, get4()+base, SEEK_SET);
      parse_tiff_ifd (base);
    }
    if (tag == 0x14 && type == 7) {
      if (len == 2560) {
	fseek (ifp, 1248, SEEK_CUR);
	goto get2_256;
      }
      fread (buf, 1, 10, ifp);
      if (!strncmp(buf,"NRW ",4)) {
	fseek (ifp, strcmp(buf+4,"0100") ? 46:1546, SEEK_CUR);
	cam_mul[0] = get4() << 2;
	cam_mul[1] = get4() + get4();
	cam_mul[2] = get4() << 2;
      }
    }
    if (tag == 0x15 && type == 2 && is_raw)
      fread (model, 64, 1, ifp);
    if (strstr(make,"PENTAX")) {
      if (tag == 0x1b) tag = 0x1018;
      if (tag == 0x1c) tag = 0x1017;
    }
    if (tag == 0x1d)
      while ((c = fgetc(ifp)) && c != EOF)
	serial = serial*10 + (isdigit(c) ? c - '0' : c % 10);
    if (tag == 0x81 && type == 4) {
      data_offset = get4();
      fseek (ifp, data_offset + 41, SEEK_SET);
      raw_height = get2() * 2;
      raw_width  = get2();
      filters = 0x61616161;
    }
    if (tag == 0x29 && type == 1) {
      c = wbi < 18 ? "012347800000005896"[wbi]-'0' : 0;
      fseek (ifp, 8 + c*32, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get4();
    }
    if ((tag == 0x81  && type == 7) ||
	(tag == 0x100 && type == 7) ||
	(tag == 0x280 && type == 1)) {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (tag == 0x88 && type == 4 && (thumb_offset = get4()))
      thumb_offset += base;
    if (tag == 0x89 && type == 4)
      thumb_length = get4();
    if (tag == 0x8c || tag == 0x96)
      meta_offset = ftell(ifp);
    if (tag == 0x97) {
      for (i=0; i < 4; i++)
	ver97 = ver97 * 10 + fgetc(ifp)-'0';
      switch (ver97) {
	case 100:
	  fseek (ifp, 68, SEEK_CUR);
	  FORC4 cam_mul[(c >> 1) | ((c & 1) << 1)] = get2();
	  break;
	case 102:
	  fseek (ifp, 6, SEEK_CUR);
	  goto get2_rggb;
	case 103:
	  fseek (ifp, 16, SEEK_CUR);
	  FORC4 cam_mul[c] = get2();
      }
      if (ver97 >= 200) {
	if (ver97 != 205) fseek (ifp, 280, SEEK_CUR);
	fread (buf97, 324, 1, ifp);
      }
    }
    if (tag == 0xa1 && type == 7) {
      order = 0x4949;
      fseek (ifp, 140, SEEK_CUR);
      FORC3 cam_mul[c] = get4();
    }
    if (tag == 0xa4 && type == 3) {
      fseek (ifp, wbi*48, SEEK_CUR);
      FORC3 cam_mul[c] = get2();
    }

    if (tag == 0xa7) {	// shutter count
      NikonKey = fgetc(ifp)^fgetc(ifp)^fgetc(ifp)^fgetc(ifp);
      if ( (unsigned) (ver97-200) < 17) {
        ci = xlat[0][serial & 0xff];
        cj = xlat[1][NikonKey];
        ck = 0x60;
        for (i=0; i < 324; i++)
          buf97[i] ^= (cj += ci * ck++);
        i = "66666>666;6A;:;55"[ver97-200] - '0';
        FORC4 cam_mul[c ^ (c >> 1) ^ (i & 1)] =
          sget2 (buf97 + (i & -2) + c*2);
      }
#ifdef LIBRAW_LIBRARY_BUILD
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
#endif
    }

    if(tag == 0xb001 && type == 3)	// Sony ModelID
      {
        unique_id = get2();
      }
    if (tag == 0x200 && len == 3)
      shot_order = (get4(),get4());
    if (tag == 0x200 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x201 && len == 4)
      goto get2_rggb;
    if (tag == 0x220 && type == 7)
      meta_offset = ftell(ifp);
    if (tag == 0x401 && type == 4 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get4();
    if (tag == 0x03d && strstr(make,"NIKON") && len == 4)
      {
        FORC4 cblack[c ^ c >> 1] = get2();
        i = cblack[3];
        FORC3 if(i>cblack[c]) i = cblack[c];
        FORC4 cblack[c]-=i;
        black += i;
      }
    if (tag == 0xe01) {		/* Nikon Capture Note */
      order = 0x4949;
      fseek (ifp, 22, SEEK_CUR);
      for (offset=22; offset+22 < len; offset += 22+i) {
	tag = get4();
	fseek (ifp, 14, SEEK_CUR);
	i = get4()-4;
	if (tag == 0x76a43207) flip = get2();
	else fseek (ifp, i, SEEK_CUR);
      }
    }
    if (tag == 0xe80 && len == 256 && type == 7) {
      fseek (ifp, 48, SEEK_CUR);
      cam_mul[0] = get2() * 508 * 1.078 / 0x10000;
      cam_mul[2] = get2() * 382 * 1.173 / 0x10000;
    }
    if (tag == 0xf00 && type == 7) {
      if (len == 614)
	fseek (ifp, 176, SEEK_CUR);
      else if (len == 734 || len == 1502)
	fseek (ifp, 148, SEEK_CUR);
      else goto next;
      goto get2_256;
    }
    if ((tag == 0x1011 && len == 9) || tag == 0x20400200)
      {
        if(!strncasecmp(make,"Olympus", 7))
          {
            int j,k;
            for (i=0; i < 3; i++)
              FORC3 adobe_cam[i][c] = ((short) get2()) / 256.0;
            for (i=0; i < 3; i++)
              for (j=0; j < 3; j++)
                for (cmatrix[i][j] = k=0; k < 3; k++)
                  cmatrix[i][j] += rgb_adobe[i][k] * adobe_cam[k][j];
          }
        else
          for (i=0; i < 3; i++)
            FORC3 cmatrix[i][c] = ((short) get2()) / 256.0;
      }
    if ((tag == 0x1012 || tag == 0x20400600) && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x1017 || tag == 0x20400100)
      cam_mul[0] = get2() / 256.0;
    if (tag == 0x1018 || tag == 0x20400100)
      cam_mul[2] = get2() / 256.0;
    if (tag == 0x2011 && len == 2) {
get2_256:
      order = 0x4d4d;
      cam_mul[0] = get2() / 256.0;
      cam_mul[2] = get2() / 256.0;
    }
    if ((tag | 0x70) == 0x2070 && (type == 4 || type == 13))
      fseek (ifp, get4()+base, SEEK_SET);
    if (tag == 0x2020)
      parse_thumb_note (base, 257, 258);
    if (tag == 0x2040)
      parse_makernote (base, 0x2040);
#ifdef LIBRAW_LIBRARY_BUILD
	// IB start
	if (tag == 0x2010)
	  {
		parse_makernote(base, 0x2010);
	  }
	// IB end
#endif
    if (tag == 0xb028) {
      fseek (ifp, get4()+base, SEEK_SET);
      parse_thumb_note (base, 136, 137);
    }
    if (tag == 0x4001 && len > 500) {
      i = len == 582 ? 50 : len == 653 ? 68 : len == 5120 ? 142 : 126;
      fseek (ifp, i, SEEK_CUR);
get2_rggb:
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
      i = len >> 3 == 164 || len == 1506 ? 112:22;
      fseek (ifp, i, SEEK_CUR);
      FORC4 sraw_mul[c ^ (c >> 1)] = get2();
    }
    if(!strcasecmp(make,"Samsung"))
      {
        if (tag == 0xa020) // get the full Samsung encryption key
            for (i=0; i<11; i++) SamsungKey[i] = get4();
        if (tag == 0xa021) // get and decode Samsung cam_mul array
            FORC4 cam_mul[c ^ (c >> 1)] = get4() - SamsungKey[c];
        if (tag == 0xa030 && len == 9)	// get and decode Samsung color matrix
            for (i=0; i < 3; i++)
              FORC3 cmatrix[i][c] = (short)((get4() + SamsungKey[i*3+c]))/256.0;
        if (tag == 0xa028)
          FORC4 cblack[c ^ (c >> 1)] = get4() - SamsungKey[c];
      }
    else
      {
        // Somebody else use 0xa021 and 0xa028?
        if (tag == 0xa021)
          FORC4 cam_mul[c ^ (c >> 1)] = get4();
        if (tag == 0xa028)
          FORC4 cam_mul[c ^ (c >> 1)] -= get4();
      }
next:
    fseek (ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}