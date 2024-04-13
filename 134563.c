void CLASS setPhaseOneFeatures (unsigned id) {

  ushort i;
  static const struct {
    ushort id;
    char t_model[32];
  } p1_unique[] = {
    // Phase One section:
    {1, "Hasselblad V"},
    {10, "PhaseOne/Mamiya"},
    {12, "Contax 645"},
    {16, "Hasselblad V"},
    {17, "Hasselblad V"},
    {18, "Contax 645"},
    {19, "PhaseOne/Mamiya"},
    {20, "Hasselblad V"},
    {21, "Contax 645"},
    {22, "PhaseOne/Mamiya"},
    {23, "Hasselblad V"},
    {24, "Hasselblad H"},
    {25, "PhaseOne/Mamiya"},
    {32, "Contax 645"},
    {34, "Hasselblad V"},
    {35, "Hasselblad V"},
    {36, "Hasselblad H"},
    {37, "Contax 645"},
    {38, "PhaseOne/Mamiya"},
    {39, "Hasselblad V"},
    {40, "Hasselblad H"},
    {41, "Contax 645"},
    {42, "PhaseOne/Mamiya"},
    {44, "Hasselblad V"},
    {45, "Hasselblad H"},
    {46, "Contax 645"},
    {47, "PhaseOne/Mamiya"},
    {48, "Hasselblad V"},
    {49, "Hasselblad H"},
    {50, "Contax 645"},
    {51, "PhaseOne/Mamiya"},
    {52, "Hasselblad V"},
    {53, "Hasselblad H"},
    {54, "Contax 645"},
    {55, "PhaseOne/Mamiya"},
    {67, "Hasselblad V"},
    {68, "Hasselblad H"},
    {69, "Contax 645"},
    {70, "PhaseOne/Mamiya"},
    {71, "Hasselblad V"},
    {72, "Hasselblad H"},
    {73, "Contax 645"},
    {74, "PhaseOne/Mamiya"},
    {76, "Hasselblad V"},
    {77, "Hasselblad H"},
    {78, "Contax 645"},
    {79, "PhaseOne/Mamiya"},
    {80, "Hasselblad V"},
    {81, "Hasselblad H"},
    {82, "Contax 645"},
    {83, "PhaseOne/Mamiya"},
    {84, "Hasselblad V"},
    {85, "Hasselblad H"},
    {86, "Contax 645"},
    {87, "PhaseOne/Mamiya"},
    {99, "Hasselblad V"},
    {100, "Hasselblad H"},
    {101, "Contax 645"},
    {102, "PhaseOne/Mamiya"},
    {103, "Hasselblad V"},
    {104, "Hasselblad H"},
    {105, "PhaseOne/Mamiya"},
    {106, "Contax 645"},
    {112, "Hasselblad V"},
    {113, "Hasselblad H"},
    {114, "Contax 645"},
    {115, "PhaseOne/Mamiya"},
    {131, "Hasselblad V"},
    {132, "Hasselblad H"},
    {133, "Contax 645"},
    {134, "PhaseOne/Mamiya"},
    {135, "Hasselblad V"},
    {136, "Hasselblad H"},
    {137, "Contax 645"},
    {138, "PhaseOne/Mamiya"},
    {140, "Hasselblad V"},
    {141, "Hasselblad H"},
    {142, "Contax 645"},
    {143, "PhaseOne/Mamiya"},
    {148, "Hasselblad V"},
    {149, "Hasselblad H"},
    {150, "Contax 645"},
    {151, "PhaseOne/Mamiya"},
    {160, "A-250"},
    {161, "A-260"},
    {162, "A-280"},
    {167, "Hasselblad V"},
    {168, "Hasselblad H"},
    {169, "Contax 645"},
    {170, "PhaseOne/Mamiya"},
    {172, "Hasselblad V"},
    {173, "Hasselblad H"},
    {174, "Contax 645"},
    {175, "PhaseOne/Mamiya"},
    {176, "Hasselblad V"},
    {177, "Hasselblad H"},
    {178, "Contax 645"},
    {179, "PhaseOne/Mamiya"},
    {180, "Hasselblad V"},
    {181, "Hasselblad H"},
    {182, "Contax 645"},
    {183, "PhaseOne/Mamiya"},
    {208, "Hasselblad V"},
    {211, "PhaseOne/Mamiya"},
    {448, "Phase One 645AF"},
    {457, "Phase One 645DF"},
    {471, "Phase One 645DF+"},
    {704, "Phase One iXA"},
    {705, "Phase One iXA - R"},
    {706, "Phase One iXU 150"},
    {707, "Phase One iXU 150 - NIR"},
    {708, "Phase One iXU 180"},
    {721, "Phase One iXR"},
    // Leaf section:
    {333,"Mamiya"},
    {329,"Universal"},
    {330,"Hasselblad H1/H2"},
    {332,"Contax"},
    {336,"AFi"},
    {327,"Mamiya"},
    {324,"Universal"},
    {325,"Hasselblad H1/H2"},
    {326,"Contax"},
    {335,"AFi"},
    {340,"Mamiya"},
    {337,"Universal"},
    {338,"Hasselblad H1/H2"},
    {339,"Contax"},
    {323,"Mamiya"},
    {320,"Universal"},
    {322,"Hasselblad H1/H2"},
    {321,"Contax"},
    {334,"AFi"},
    {369,"Universal"},
    {370,"Mamiya"},
    {371,"Hasselblad H1/H2"},
    {372,"Contax"},
    {373,"Afi"},
  };
  if (id && !imgdata.lens.makernotes.body[0]) {
    for (i=0; i < sizeof p1_unique / sizeof *p1_unique; i++)
      if (id == p1_unique[i].id) {
        strcpy(imgdata.lens.makernotes.body,p1_unique[i].t_model);
      }
  }
  return;
}