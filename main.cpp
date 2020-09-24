////////////////////////////////////////
////                                ////
////       Maximiliano Aubel        ////
////           13/9/2020            ////
////                                ////
////        GAMMA CORRECTION        ////
////    Digital Image Processing    ////
////         See README.md          ////
////                                ////
////////////////////////////////////////

#include <stdio.h>
#include <iostream> 
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <opencv2/opencv.hpp>

using namespace std;

cv::Mat lookUpTable(1, 256, CV_8U);
uchar* p = lookUpTable.ptr();
std::chrono::steady_clock::time_point first = std::chrono::steady_clock::now();
int frames = 0;


cv::Mat addBorder(cv::Mat &img, int r, int g, int b)
{   
    int top  = (int) (0.03*img.rows);
    int left = (int) (0.03*img.cols);
    int bottom = top;
    int right = left;

    cv::Mat res;
    cv::Scalar value( b, g, r );  // B G R

    copyMakeBorder( img, res, top, bottom, left, right, cv::BORDER_CONSTANT, value );
    return res;
}

cv::Mat gammaCorrection(cv::Mat &img, float gamma, bool m1, bool m2, bool f, 
                    int x, int y, int w, int h)
{
    
    if(f)
    {   
        cv::Mat insetImage(img, cv::Rect(x, y, w, h));
        cv::Mat ROI = gammaCorrection(insetImage, gamma, m1, m2, false, x, y, w, h);
        ROI.copyTo(insetImage);
        return img;
    }

    if(m1) // with lookuptable
    {
        cv::Mat res = img.clone();
        LUT(img, lookUpTable, res);
        frames ++;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int time = chrono::duration_cast<std::chrono::microseconds> (end - first).count();
        float avg = time/frames;
        printf("Tiempo promedio por frame: %f [us]\n", avg);

        return  res;
    }

    else if(m2) // pixel by pixel
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        cv::Mat res = img;
        int depth = img.depth(),
        channels = img.channels(),
        step = img.step;
        uchar *data = img.data;
        cv::Mat_<cv::Vec3b> _I = img;
        for(int i = 0; i < _I.rows; ++i)
        for(int j = 0; j < _I.cols; ++j)
        for(int k = 0; k < 3; k++)
            _I(i,j)[k] = (int)(255 * pow( _I(i,j)[k]/255.0, gamma));
        img = _I;

        frames++;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int time = chrono::duration_cast<std::chrono::microseconds> (end - first).count();
        float avg = time/frames;
        printf("Tiempo promedio por frame: %f [us]\n", avg);

        return img;
    }

    else return img;
}

void webcam(float gamma, bool m1, bool m2, int x, int y, 
                    int w, int h, int r, int g, int b, bool c, bool f){

    cv::Mat frame, frame2;
    cv::VideoCapture camera(0);
    string caption = "capturing from webcam with gamma: " + to_string(gamma);
    while (1) 
    {
        camera >> frame;
        frame = gammaCorrection(frame, gamma, m1, m2, f, x, y, w, h);
        if(c) frame = addBorder(frame, r, g, b);
        cv::imshow(caption, frame);
        if (cv::waitKey(10) >= 0)
            return;
    }
    return;
}

void single_image(string file, float gamma, bool m1, bool m2, int x, int y, 
                    int w, int h, int r, int g, int b, bool c, bool f)
{
    cv::Mat image = cv::imread( file, 1 );
    string caption = file + " with gamma: " + to_string(gamma);
    if ( !image.data )
    {
        printf("No image data \n");
        return;
    }
    cv::namedWindow(caption, cv::WINDOW_AUTOSIZE );
    image = gammaCorrection(image, gamma, m1, m2, f, x, y, w, h);
    if(c) image = addBorder(image, r, g, b);
    cv::imshow(caption, image);
    cv::waitKey(0);
    return;
}


int main(int argc, char** argv )
{
    bool m1 = false, m2 = false, v = false, i = false, f = false, c = false;
    string file;
    int x, y, w, h, r, g, b;
    float gamma;

    if ( argc < 2 )
    {
        printf("usage:\n");
        printf("./GAMMA [-m1 | -m2] -v gamma [-f x y w h] [-c r g b]\n./GAMMA [-m1 | -m2] -i imagen gamma [-f x y w h] [-c r g b]\n\n");
        printf("-m1: usar tabla precalculada para la transformación gamma.\n");
        printf("-m2: implementar el procesamiento pixel a pixel de gamma (sin tabla).\n");
        printf("-v: usar webcam.\n");
        printf("-i: usar imagen.\n");
        printf("image: Ruta de imagen a procesar.\n");
        printf("gamma: Nivel de Gamma. Debe ser un valor real, mayor que 0.\n");
        printf("-f: (Optional)\tDefine área a la que aplica gamma. Resto es borde. Se define rectángulo posición (x,y) y tamaño w X h.\n");
        printf("\t\tSi no se define, se usa imagen completa.\n");
        printf("-c: (Optional) \tColor (r,g,b) del borde. Si no se define, se usa negro.\n");
        return -1;
    }
    
    for( int j = 1; j < argc; j++)
    {

        if( !strcmp(argv[j], "-m1") ) m1 = true;
        if( !strcmp(argv[j], "-m2") ) m2 = true;
        
        if( !strcmp(argv[j], "-v"))
        {
            v = true;
            gamma = strtof(argv[j + 1], NULL);
        }

        if( !strcmp(argv[j], "-i") )
        {            
            i = true;
            file = argv[j + 1];
            gamma = strtof(argv[j + 2], NULL);
        }

        if( !strcmp(argv[j], "-f") )
        {
            f = true;
            x = strtol(argv[j + 1], NULL, 10);
            y = strtol(argv[j + 2], NULL, 10);
            w = strtol(argv[j + 3], NULL, 10);
            h = strtol(argv[j + 4], NULL, 10);
        }
        
        if( !strcmp(argv[j], "-c") )
        {
            c = true;
            if (argv[j + 1] != NULL)
            {
                r = strtol(argv[j + 1], NULL, 10);
                g = strtol(argv[j + 2], NULL, 10);
                b = strtol(argv[j + 3], NULL, 10);
            }
            else 
                r=0,g=0,b=0;
        }
    }

    for( int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow((i / 255.0), gamma) * 255.0);

    first = std::chrono::steady_clock::now();

    if (v) webcam(gamma, m1, m2, x, y, w, h, r, g, b, c, f);
    else if(i) single_image(file, gamma, m1, m2, x, y, w, h, r, g, b, c, f);
 
    return 0;
}