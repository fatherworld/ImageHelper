#include "Config.h"

#include <stdlib.h>
#include "stdio_fl.h"
#include "memory_fl.h"

#define LOG_TAG "ImageResize"
#include "Log.h"
#include "ErrorTools.h"

#include "ImageResize.h"

#define CLAMP(v, minv, maxv) (v) > (maxv) ? (maxv) : ((v) < (minv) ? (minv) : (v))

int ImageResize(unsigned char* igray, unsigned char* ogray, int iwidth, int iheight, int owidth, int oheight) {

    float wScale = iwidth * 1.f / owidth;
    float hScale = iheight * 1.f / oheight;
    for (int i = 0; i < oheight; i++) {
        for (int j = 0; j < owidth; j++) {

            float outHY = i * hScale;
            float outWX = j * wScale;

            int smallY = (int)outHY;
            int bigY = CLAMP(smallY + 1, 0, iheight - 1);
            int smallX = (int)outWX;
            int bigX = CLAMP(smallX + 1, 0, iwidth - 1);

            float xP1 = outWX - smallX;
            float xP2 = 1.f - xP1;
            float yP1 = outHY - smallY;
            float yP2 = 1.f - yP1;

            ogray[i * owidth + j] = (unsigned char)(CLAMP(yP2 * (xP1 * igray[smallY * iwidth + bigX] +
                xP2 * igray[smallY * iwidth + smallX]) +
                yP1 * (xP1 * igray[bigY * iwidth + bigX] +
                    xP2 * igray[bigY * iwidth + smallX]),
                0, 255.f));
        }
    }
    return 0;
}
