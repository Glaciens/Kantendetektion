#include "opencv2/opencv.hpp"
#include "iostream"
#include "cmath"
#include "chrono"
#include "ctime"

#define LENGTH 480
#define WIDTH 640
#define MATRIX 5
#define LEVEL 20

int main()
{
    int gx,gy;
    const int ma2 = MATRIX * MATRIX;
    int value[ma2],dual,count; 
    const int m2=(ma2-1)>>1, m1=(MATRIX-1)>>1;
    const int sx[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
    const int sy[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
    const int *ptr_end = &value[ma2];

    printf("load Image: ready\n");
    cv::Mat img  = cv::imread("No-Filter.png");
    uint8_t frame[LENGTH][WIDTH][3]={0};
    uint8_t gray[LENGTH][WIDTH]={0};
    uint8_t median[LENGTH][WIDTH]={0};
    uint8_t gxy[LENGTH][WIDTH]={0};

    for(int y=LENGTH;y--;)
        for(int x=WIDTH;x--;)
            for(int i=3;i--;)
               *(*(*(frame+y)+x)+i)=img.at<cv::Vec3b>(y,x)[i];

    cv::Mat frame_m = cv::Mat(LENGTH,WIDTH,CV_8UC3,&frame);
    cv::imwrite("img/No-Filter.png", frame_m);


    auto start = std::chrono::system_clock::now();

    for(int y=LENGTH;y--;)
        for(int x=WIDTH;x--;)//0=blue 1=green 2=red
            *(*(gray+y)+x) = (*(*(*(frame+y)+x)+0)>>3) + (*(*(*(frame+y)+x)+1)>>1) +( *(*(*(frame+y)+x)+2)>>2);
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    printf("gray-Filter: ready %f s\n", elapsed_seconds.count());
    
    cv::Mat gray_m = cv::Mat(LENGTH,WIDTH,CV_8U,&gray);
    cv::imwrite("img/gray.png", gray_m);

    auto start2 = std::chrono::system_clock::now();
    for(int y=MATRIX;y!=LENGTH-MATRIX;y++)
        for(int x=MATRIX;x!=WIDTH-MATRIX;x++)
        {
            for(int i=1;i!=MATRIX+1;i++)
                for(int n=1;n!=MATRIX+1;n++)
                    *(value+i*n-1) = *(*(gray+y-n+m1)+x-i+m1);

            for(int *num = value; num != ptr_end; num++)
            {
                count=0;
                dual=-1;
                for(int *com = value; com != ptr_end; com++)
                {
                    if(count>m2) break;
                    if(*com>*num) count++;
                    if(*com == *num) dual++;
                }
            
                if(count + dual >= m2 && count <= m2)
                {
                    *(*(median+y)+x) = *num;
                    break;
                }
            }
        }

    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    printf("median-Filter: ready %f s\n",elapsed_seconds2.count());
    cv::Mat median_m = cv::Mat(LENGTH,WIDTH,CV_8U,&median);
    cv::imwrite("img/median.png", median_m);

    auto start3 = std::chrono::system_clock::now();
    for(int x=1;x!=WIDTH-1;x++)
        for(int y=1;y!=LENGTH-1;y++)
        {
            gx=0;
            gy=0;
            for(int i=0;i!=3;i++)
                for(int n=0;n!=3;n++)
                {
                   gx += sx[i][n]* *(*(median+y-1+i)+x-1+n);
                   gy += sy[i][n]* *(*(median+y-1+i)+x-1+n);
                }
            //*(*(gxy+y)+x) = (int)(std::sqrt(std::pow(gx,2) + std::pow(gy,2)))>>2;
            *(*(gxy+y)+x) = (abs(gx)+abs(gy))>>1;
            if(*(*(gxy+y)+x) > LEVEL) *(*(gxy+y)+x)=255;
            else *(*(gxy+y)+x) = 0;
        }

    auto end3 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds3 = end3-start3;
    printf("Sobel XY-Filter: ready %f s\n", elapsed_seconds3.count());
    cv::Mat gxy_m = cv::Mat(LENGTH,WIDTH,CV_8U,&gxy);
    cv::imwrite("img/sobel.png",gxy_m);
    //cv::imshow("IMAGE",gxy);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

