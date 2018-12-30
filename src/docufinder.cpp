#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

Mat edges_detect(Mat img, int min_val, int max_val) {
    Mat gray, gray1, gray2, blured, blured1, edged, edged1;
    cvtColor(img, gray, COLOR_RGB2GRAY);
    bilateralFilter(gray, gray1, 9, 75, 75);
    adaptiveThreshold(gray1, gray2, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 115, 4);
    medianBlur(gray2,blured, 11);
    Scalar value = Scalar(0,0,0);
    copyMakeBorder(blured, blured1, 5, 5, 5, 5, BORDER_CONSTANT, value);
    Canny(blured1, edged, min_val, max_val);
    //Close gaps between edges (double page clouse => rectangle kernel)
    Mat element;
    int element_shape = MORPH_RECT;
    element = getStructuringElement(element_shape, Size(5, 11 ), Point(1, 1) );
    morphologyEx( edged, edged1, MORPH_CLOSE, element );
    return edged1;

}

Mat resize(Mat img, int height=100, bool allways=false) {
    if (img.rows > height || allways) {
        double ratio = height / img.rows;
        return resize(img, (int)(ratio*img.cols), height);
    }
    return img;
}

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    Mat image;
    image = imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Original Image", WINDOW_AUTOSIZE );
    imshow("Original Image", image);
    Mat orig;
    orig = image;
    // cvtColor(orig, gray, COLOR_RGB2GRAY);
    // //GaussianBlur(gray, blured, Size(7, 7), 0);
    // blured=gray;
    // Canny(blured, edged, 0, 100);

    //
    Mat edged = edges_detect(image, 200, 250);
    std::vector<Vec4i> hierarchy;
    std::vector<std::vector<Point> > contours;
    findContours(edged, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    int largest_contour_index=0;
    int largest_area=0;
    Rect bounding_rect;
    Scalar color( 255,255,255);
    for( int i = 0; i< contours.size(); i++ )
    {

        //  Find the area of contour
        double a=contourArea( contours[i],false);
        if(a>largest_area){
            largest_area=a;cout<<i<<" area  "<<a<<endl;
            // Store the index of largest contour
            largest_contour_index=i;
            // Find the bounding rectangle for biggest contour
            bounding_rect=boundingRect(contours[i]);
        }
    }

    drawContours( orig, contours, largest_contour_index, color, FILLED,8,hierarchy);
    rectangle(orig, bounding_rect,  Scalar(0,255,0),2, 8,0);

    namedWindow( "Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", orig);
    waitKey(0);

    return 0;
}
