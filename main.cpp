#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include "include/easyexif/exif.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

using namespace std;

const int BATCH_SIZE = 100;

vector<string> read_directory(char *input_folder) {
    DIR *dir;
    struct dirent *ent;
    vector<string> filenames;

    dir = opendir(input_folder);
    if (dir == NULL) {
        cerr << "Error: Can't open folder " << input_folder << endl;
        return filenames;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            char *filename = ent->d_name;
            if (strstr(filename, ".jpg") != NULL || strstr(filename, ".JPG") != NULL) {
                filenames.push_back(string(filename));
            }
        }
    }
    closedir(dir);
    return filenames;
}

void process_batch(const vector<string> &batch, const string &input_folder, const string &temp_csv) {
    ofstream fout(temp_csv);

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

    for (const string &filename : batch) {
        try {
            string path = input_folder + "/" + filename;

            FILE *fp = fopen(path.c_str(), "rb");
            if (!fp) {
                cerr << "Error: Can't open file " << path << endl;
                continue;
            }

            fseek(fp, 0, SEEK_END);
            unsigned long fsize = ftell(fp);
            rewind(fp);
            unsigned char *buf;
            buf = new unsigned char[fsize];
            if (fread(buf, 1, fsize, fp) != fsize) {
                cerr << "Error: Can't read file " << path << endl;
                delete[] buf;
                fclose(fp);
                continue;
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

            // Check if ImageHeight and ImageWidth are present and not 0, if not, compute the aspect ratio
            if (result.ImageHeight == 0 || result.ImageWidth == 0) {
                int width, height, channels;
                if (!stbi_info(path.c_str(), &width, &height, &channels)) {
                    cerr << "Error: Can't open file " << path << endl;
                    continue;
                }
                result.ImageHeight = height;
                result.ImageWidth = width;
            }

            string output_string = "";
            output_string += filename + ",";
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
            fout.flush();

        } catch (exception &e) {
            // Continue if there is an error
            cerr << "Error: " << e.what() << endl;
            // skip this file
            continue;
        }
    }

    fout.close();
}

void merge_temp_csvs(const vector<string> &temp_csvs, const string &output_file) {
    ofstream fout(output_file);
    bool header_written = false;

    for (const string &temp_csv : temp_csvs) {
        ifstream fin(temp_csv);

        string line;
        while (getline(fin, line)) {
            if (!header_written) {
                fout << line << endl;
                header_written = true;
            } else if (!line.empty()) {
                fout << line << endl;
            }
        }

        fin.close();
        remove(temp_csv.c_str());
    }

    fout.close();
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " input_folder output_file.csv" << endl;
        return 1;
    }

    string input_folder = argv[1];
    string output_file = argv[2];

    vector<string> filenames = read_directory(argv[1]);
    vector<string> temp_csvs;

    for (size_t i = 0; i < filenames.size(); i += BATCH_SIZE) {
        vector<string> batch(filenames.begin() + i, min(filenames.begin() + i + BATCH_SIZE, filenames.end()));
        string temp_csv = "temp_" + to_string(i / BATCH_SIZE) + ".csv";
        temp_csvs.push_back(temp_csv);
        process_batch(batch, input_folder, temp_csv);
    }

    merge_temp_csvs(temp_csvs, output_file);

    return 0;
}