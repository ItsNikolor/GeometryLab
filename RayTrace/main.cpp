#include <iostream>
#include <cstdint>

#include <string>
#include <vector>
#include <unordered_map>

#include "Bitmap.h"
#include "render.h"

const uint32_t RED   = 0x000000FF;
const uint32_t GREEN = 0x0000FF00;
const uint32_t BLUE  = 0x00FF0000;

int main(int argc, const char** argv)
{
    std::unordered_map<std::string, std::string> cmdLineParams;

    for (int i = 0; i < argc; i++)
    {
        std::string key(argv[i]);

        if (key.size() > 0 && key[0] == '-')
        {
            if (i != argc - 1) // not last argument
            {
                cmdLineParams[key] = argv[i + 1];
                i++;
            }
            else
                cmdLineParams[key] = "";
        }
    }

    std::string outFilePath = "zout.bmp";
    if (cmdLineParams.find("-out") != cmdLineParams.end())
        outFilePath = cmdLineParams["-out"];

    int sceneId = 0;
    if (cmdLineParams.find("-scene") != cmdLineParams.end())
        sceneId = atoi(cmdLineParams["-scene"].c_str());

    int threads = 4;
    if (cmdLineParams.find("-threads") != cmdLineParams.end())
        threads = atoi(cmdLineParams["-threads"].c_str());


    int w = 716, h = 716;
    std::vector<uint32_t> image(w * h);

    if (sceneId == 1 || sceneId == 2) {
        render(image, w, h, threads, sceneId);
        SaveBMP(outFilePath.c_str(), image.data(), w, h);
    }
    std::cout << "end." << std::endl;
    return 0;
}
