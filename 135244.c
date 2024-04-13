std::shared_ptr<GfxColorTransform> GfxState::getXYZ2DisplayTransform()
{
    auto transform = XYZ2DisplayTransformRelCol;
    if (strcmp(renderingIntent, "AbsoluteColorimetric") == 0) {
        transform = XYZ2DisplayTransformAbsCol;
    } else if (strcmp(renderingIntent, "Saturation") == 0) {
        transform = XYZ2DisplayTransformSat;
    } else if (strcmp(renderingIntent, "Perceptual") == 0) {
        transform = XYZ2DisplayTransformPerc;
    }
    return transform;
}