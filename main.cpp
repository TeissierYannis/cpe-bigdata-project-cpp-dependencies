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

    fout
            << "filename,Make,Model,Software,BitsPerSample,ImageWidth,ImageHeight,ImageDescription,Orientation,Copyright,DateTime,DateTimeOriginal,DateTimeDigitized,SubSecTimeOriginal,ExposureTime,FNumber,ExposureProgram,ISOSpeedRatings,SubjectDistance,ExposureBiasValue,Flash,FlashReturnedLight,FlashMode,MeteringMode,FocalLength,FocalLengthIn35mm,Latitude,LatitudeDegrees,LatitudeMinutes,LatitudeSeconds,LatitudeDirection,Longitude,LongitudeDegrees,LongitudeMinutes,LongitudeSeconds,LongitudeDirection,Altitude,DOP,FocalLengthMin,FocalLengthMax,FStopMin,FStopMax,LensMake,LensModel,FocalPlaneXResolution,FocalPlaneYResolution"
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

                    fout << filename << ",";
                    fout << result.Make.c_str() << ",";
                    fout << result.Model.c_str() << ",";
                    fout << result.Software.c_str() << ",";
                    fout << result.BitsPerSample << ",";
                    fout << result.ImageWidth << ",";
                    fout << result.ImageHeight << ",";
                    fout << result.ImageDescription.c_str() << ",";
                    fout << result.Orientation << ",";
                    fout << result.Copyright.c_str() << ",";
                    fout << result.DateTime.c_str() << ",";
                    fout << result.DateTimeOriginal.c_str() << ",";
                    fout << result.DateTimeDigitized.c_str() << ",";
                    fout << result.SubSecTimeOriginal.c_str() << ",";
                    fout << result.ExposureTime << ",";
                    fout << result.FNumber << ",";
                    fout << result.ExposureProgram << ",";
                    fout << result.ISOSpeedRatings << ",";
                    fout << result.SubjectDistance << ",";
                    fout << result.ExposureBiasValue << ",";
                    fout << result.Flash << ",";
                    fout << result.FlashReturnedLight << ",";
                    fout << result.FlashMode << ",";
                    fout << result.MeteringMode << ",";
                    fout << result.FocalLength << ",";
                    fout << result.FocalLengthIn35mm << ",";
                    fout << result.GeoLocation.Latitude << ",";
                    fout << result.GeoLocation.LatComponents.degrees << ",";
                    fout << result.GeoLocation.LatComponents.minutes << ",";
                    fout << result.GeoLocation.LatComponents.seconds << ",";
                    fout << result.GeoLocation.LatComponents.direction << ",";
                    fout << result.GeoLocation.Longitude << ",";
                    fout << result.GeoLocation.LonComponents.degrees << ",";
                    fout << result.GeoLocation.LonComponents.minutes << ",";
                    fout << result.GeoLocation.LonComponents.seconds << ",";
                    fout << result.GeoLocation.LonComponents.direction << ",";
                    fout << result.GeoLocation.Altitude << ",";
                    fout << result.GeoLocation.DOP << ",";
                    fout << result.LensInfo.FocalLengthMin << ",";
                    fout << result.LensInfo.FocalLengthMax << ",";
                    fout << result.LensInfo.FStopMin << ",";
                    fout << result.LensInfo.FStopMax << ",";
                    fout << result.LensInfo.Make.c_str() << ",";
                    fout << result.LensInfo.Model.c_str() << ",";
                    fout << result.LensInfo.FocalPlaneXResolution << ",";
                    fout << result.LensInfo.FocalPlaneYResolution << endl;

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