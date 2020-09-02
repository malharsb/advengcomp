#include "simplebitmap.h"
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[])
{
    // Case when there's no argument 
    if (argv[1]==NULL)
    {
        cout << "Usage: ps4_1 <pngFileName.png>\n";
        return 0; 
    }

    // Define input empty bitmap
    SimpleBitmap inputBm;
    bool isLoaded = inputBm.LoadPng(argv[1]);

    if (isLoaded==false)
    {
        cout << "Error: Failed to read a .PNG file.\n";
        return 0;
    }

    int counter=0;
    int width = inputBm.GetWidth();
    int height = inputBm.GetHeight();
    int filter = 40;

    for (int j=0; j<height; j+=filter)
    {
        for (int i=0; i<width; i+=filter)
        {
            if (counter==200) 
                return 0;

            SimpleBitmap cutBm = inputBm.CutOut(i,j,filter,filter);
            string name = to_string(counter) + ".png";
            const char *name_ptr = name.c_str();

            counter++;

            FILE *fp;
            fp = fopen(name_ptr, "wb");
            cutBm.SavePng(fp);
            fclose(fp);
            
        }
    }
}
