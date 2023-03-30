#include <iostream>
#include <fstream>
#include <dirent.h>
#include "include/easyexif/exif.h"

#define STB_IMAGE_IMPLEMENTATION

#include "include/stb_image.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " input_folder output_file.csv" << endl;
        return 1;
    }

    char *input_folder = argv[1];
    char *output_file = argv[2];

    DIR *dir;
    struct dirent *ent;

    dir = opendir(input_folder);

    if (dir == NULL) {
        cerr << "Error: Can't open folder " << input_folder << endl;
        return 1;
    }

    ofstream fout(output_file);

    fout << "filename,"
            "Make,"
            "Model,"
            "Software,"
            "BitsPerSample,"
            "ImageWidth,"
            "ImageHeight,"
            "ImageDescription,"
            "Orientation,"
            "Copyright,"
            "DateTime,"
            "DateTimeOriginal,"
            "DateTimeDigitized,"
            "SubSecTimeOriginal,"
            "ExposureTime,"
            "FNumber,"
            "ExposureProgram,"
            "ISOSpeedRatings,"
            "SubjectDistance,"
            "ExposureBiasValue,"
            "Flash,"
            "FlashReturnedLight,"
            "FlashMode,"
            "MeteringMode,"
            "FocalLength,"
            "FocalLengthIn35mm,"
            "Latitude,"
            "LatitudeDegrees,"
            "LatitudeMinutes,"
            "LatitudeSeconds,"
            "LatitudeDirection,"
            "Longitude,"
            "LongitudeDegrees,"
            "LongitudeMinutes,"
            "LongitudeSeconds,"
            "LongitudeDirection,"
            "Altitude,"
            "DOP,"
            "FocalLengthMin,"
            "FocalLengthMax,"
            "FStopMin,"
            "FStopMax,"
            "LensMake,"
            "LensModel,"
            "FocalPlaneXResolution,"
            "FocalPlaneYResolution"
         << endl;
    // loop through all files in the folder
    while ((ent = readdir(dir)) != NULL) {

        if (ent->d_type == DT_REG) {
            char *filename = ent->d_name;
            if (strstr(filename, ".jpg") != NULL || strstr(filename, ".JPG") != NULL) {
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", input_folder, filename);

                FILE *fp = fopen(path, "rb");
                if (!fp) {
                    cerr << "Error: Can't open file " << path << endl;
                    return -1;
                }

                fseek(fp, 0, SEEK_END);
                unsigned long fsize = ftell(fp);
                rewind(fp);
                unsigned char *buf;
                buf = new unsigned char[fsize];
                if (fread(buf, 1, fsize, fp) != fsize) {
                    cerr << "Error: Can't read file " << path << endl;
                    delete[] buf;
                    return -2;
                }
                fclose(fp);

                // Parse EXIF
                easyexif::EXIFInfo result;
                int code = result.parseFrom(buf, fsize);
                delete[] buf;
                if (code) {
                    cerr << "Error parsing EXIF: code " << code << endl;
                    continue;
                }

                try {

                    // Check if ImageHeight and ImageWidth are present and not 0, if not, compute the aspect ratio
                    if (result.ImageHeight == 0 || result.ImageWidth == 0) {
                        int width, height, channels;
                        if (!stbi_info(path, &width, &height, &channels)) {
                            cerr << "Error: Can't open file " << path << endl;
                            return -1;
                        }
                        result.ImageHeight = height;
                        result.ImageWidth = width;
                    }

                    string output_string = "";
                    output_string += string(filename) + ",";
                    output_string += result.Make + ",";
                    output_string += result.Model + ",";
                    output_string += result.Software + ",";
                    output_string += to_string(result.BitsPerSample) + ",";
                    output_string += to_string(result.ImageWidth) + ",";
                    output_string += to_string(result.ImageHeight) + ",";
                    output_string += result.ImageDescription + ",";
                    output_string += to_string(result.Orientation) + ",";
                    output_string += result.Copyright + ",";
                    output_string += result.DateTime + ",";
                    output_string += result.DateTimeOriginal + ",";
                    output_string += result.DateTimeDigitized + ",";
                    output_string += result.SubSecTimeOriginal + ",";
                    output_string += to_string(result.ExposureTime) + ",";
                    output_string += to_string(result.FNumber) + ",";
                    output_string += to_string(result.ExposureProgram) + ",";
                    output_string += to_string(result.ISOSpeedRatings) + ",";
                    output_string += to_string(result.SubjectDistance) + ",";
                    output_string += to_string(result.ExposureBiasValue) + ",";
                    output_string += to_string(result.Flash) + ",";
                    output_string += to_string(result.FlashReturnedLight) + ",";
                    output_string += to_string(result.FlashMode) + ",";
                    output_string += to_string(result.MeteringMode) + ",";
                    output_string += to_string(result.FocalLength) + ",";
                    output_string += to_string(result.FocalLengthIn35mm) + ",";
                    output_string += to_string(result.GeoLocation.Latitude) + ",";
                    output_string += to_string(result.GeoLocation.LatComponents.degrees) + ",";
                    output_string += to_string(result.GeoLocation.LatComponents.minutes) + ",";
                    output_string += to_string(result.GeoLocation.LatComponents.seconds) + ",";
                    output_string += to_string(result.GeoLocation.LatComponents.direction) + ",";
                    output_string += to_string(result.GeoLocation.Longitude) + ",";
                    output_string += to_string(result.GeoLocation.LonComponents.degrees) + ",";
                    output_string += to_string(result.GeoLocation.LonComponents.minutes) + ",";
                    output_string += to_string(result.GeoLocation.LonComponents.seconds) + ",";
                    output_string += to_string(result.GeoLocation.LonComponents.direction) + ",";
                    output_string += to_string(result.GeoLocation.Altitude) + ",";
                    output_string += to_string(result.GeoLocation.DOP) + ",";
                    output_string += to_string(result.LensInfo.FocalLengthMin) + ",";
                    output_string += to_string(result.LensInfo.FocalLengthMax) + ",";
                    output_string += to_string(result.LensInfo.FStopMin) + ",";
                    output_string += to_string(result.LensInfo.FStopMax) + ",";
                    output_string += result.LensInfo.Make + ",";
                    output_string += result.LensInfo.Model + ",";
                    output_string += to_string(result.LensInfo.FocalPlaneXResolution) + ",";
                    output_string += to_string(result.LensInfo.FocalPlaneYResolution);

                    fout << output_string << endl;

                } catch (exception &e) {
                    cerr << "Error: " << e.what() << endl;
                }

                fout.flush();

            }
        }
    }

    closedir(dir);
    fout.close();

    return 0;
}
