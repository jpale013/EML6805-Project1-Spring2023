#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

void info(void) {
    std::cout<< "EML 6805 Robot Design" << std::endl
            << "Florida International Univeristy" <<std::endl
            << "Department of Mechanical and Materials Engneering" << std::endl
            << "Juan Palenzuela <jpale013@fiu.edu>" << std::endl
            << "Miami, Spring 2023" << std::endl
            << std::endl;
}




#define KEY_ESC 27

using Pixel = cv::Vec<uchar, 3>; // G,B,R

uchar gray(Pixel p) {
    return 0.3*p[2] + 0.59*p[1] + 0.11*p[0];
}

void plotRow(cv::Mat &image, std::vector<uchar> row_colors, int y, float scale, cv::Scalar color) {
    std::vector<cv::Point> points;
    for (int x = 0; x < row_colors.size(); ++x)
        points.push_back( cv::Point(x, y - scale*row_colors[x]) );
    cv::polylines(image, points, false, color, 2);
}

int main(int argc, char** argv) {
    info();
    // Create a capture object from device numer (or video filename)
    cv::VideoCapture cap("../line.mp4");
    //cv::VideoCapture cap(0);
    //check for errors
    if(!cap.isOpened()){
        std::cout << "Oops, capture cannot be created!" << std::endl;
        return -1;
    }

    bool run1 = true;
    bool run2 = true;

    int pt1 = 0;
    int pt2 = 0;
    int middlepoint = 0;
    int minblue = 200;

    // Create windows with trackbar
    cv::namedWindow("EML4840");
    // Trackers
    int track_row = 70;     // Percentage
    int track_scale = 40;   // Percentage
    int track_resize = 100;
    cv::createTrackbar("Row", "EML4840", &track_row, 100);
    cv::createTrackbar("Scale", "EML4840", &track_scale, 100);
    cv::createTrackbar("resize", "EML4840", &track_resize, 100);
    // Menu
    bool show_red = true;
    bool show_blue = true;
    bool show_green = true;
    bool show_gray = true;
    std::cout << "Press:" << std::endl
              << "s            : to save image" << std::endl
              << "r, g, b, or k: to show colors" << std::endl
              << "q or ESC     : to quit" << std::endl;
    // Create vectors to store the graphs
    std::vector<uchar> r, g, b, k;
    // Run until 'q' is pressed...
    bool running = true;

    std::cout << "Press 'Esc' to quit..." << std::endl;
    while(true) {
        //create an image object
        cv::Mat image;
        //capture frame
        bool okay = cap.read( image );
        //skip if any capture error happened (or video eded)
        std::string type = cv::typeToString( image.type() );

        if (type != "CV_8UC3") {
        std::cout << "Ops, format '" << type << "' not supported!" << std::endl;
        return 1;
         }
        // if(okay)
        //     cv::imshow("Image", image);
        //break loop if 'ESC' is pressed
        if(cv::waitKey(30) == KEY_ESC)
            break;

        r.clear();
        g.clear();
        b.clear();
        k.clear();
        // Update scale
        float scale = 0.01*track_scale;

        cv::Mat canvas = image.clone();

        int y = 0.01*track_row*(image.rows-1);
        for (int x = 0; x < image.cols; ++x ) {
            Pixel pixel = image.at<Pixel>( cv::Point(x, y) );
            r.push_back( pixel[2] );
            g.push_back( pixel[1] );
            b.push_back( pixel[0] );
            k.push_back( gray(pixel) );

            if (pixel[0] > minblue && pixel[1] < minblue && pixel[2] < minblue && run1)
            {
                run1 = false;
                run2 = true;
                pt1 = x;
              
            }

            if (pixel[0] < minblue && !run1 && run2)
            {
                
                run2 = false;
                run1 = true;
                pt2 = x;

            }

            middlepoint = pt1 + ((pt2 - pt1)/2);

            cv::circle(canvas, cv::Point(middlepoint,y), 5, cv::Scalar(50,255,0), 2);


        }

        // cv::Mat canvas = image.clone();
         if (show_red)   plotRow(canvas, r, y, scale, cv::Scalar(0,0,255));
         if (show_green) plotRow(canvas, g, y, scale, cv::Scalar(0,255,0));
         if (show_blue)  plotRow(canvas, b, y, scale, cv::Scalar(255,0,0));
         if (show_gray)  plotRow(canvas, k, y, scale, cv::Scalar(0,0,0));
         cv::line(canvas, cv::Point(0, y), cv::Point(image.cols, y), cv::Scalar(0,0,0), 2);

         cv::line(canvas, cv::Point((image.cols/2)-10, y), cv::Point((image.cols/2)+10, y), cv::Scalar(255,255,255), 1);
         cv::line(canvas, cv::Point((image.cols/2), y-10), cv::Point((image.cols/2), y+10), cv::Scalar(255,255,255), 1);

        int key = cv::waitKey(10);
        switch(key) {
        case 's':
            cv::imwrite("../output.jpg", image);
            break;
        case 'q':
        case KEY_ESC:
            running = false;
            break;
        case 'r':
            show_red = !show_red;
            break;
        case 'g':
            show_green = !show_green;
            break;
        case 'b':
            show_blue = !show_blue;
            break;
        case 'k':
            show_gray = !show_gray;
            break;
        }
        // Show image
        cv::resize(canvas, canvas, cv::Size(), 0.01*track_resize, 0.01*track_resize);
        cv::imshow("EML4840", canvas);
    }
    
    // Close all windows
    cv::destroyAllWindows();
    return 0;
}

