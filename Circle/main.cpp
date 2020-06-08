#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;

vector<Vec3f> STEP_1(Mat readImage);
void STEP_2345(vector<Vec3f> circles);
float BilinearInterpolation(float row, float col);

#define LINEAMOUNT 200
#define Pi 3.1415926

Mat srcImage, Image;

int main(void)
{
    Image = imread("./brake_disk_part_06.png");
    cvtColor(Image, srcImage, COLOR_BGR2GRAY);
    vector<Vec3f> circles = STEP_1(srcImage);
    STEP_2345(circles);
    float** ACcircle = new float* [circles.size()];

    imshow("result", Image);


    waitKey(0);
    return 0;
}

/*
 * step1：图像分割得到大致的圆心和半径
 */
vector<Vec3f> STEP_1(Mat readImage)
{
    Mat edgeImage, srcImage;
    GaussianBlur(readImage, srcImage, Size(9, 9), 2, 2);
    Canny(srcImage, edgeImage, 100, 35, 3, false);
    imshow("edgeImage", edgeImage);
    vector<Vec3f> circles;
    HoughCircles(edgeImage, circles, HOUGH_GRADIENT, 1, edgeImage.rows / 5, 100, 35, 0, 0);

    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        //绘制圆心
        circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        //绘制圆轮廓
        circle(srcImage, center, radius, Scalar(255, 50, 0), 3, 8, 0);
    }
    imshow("分割结果", srcImage);
    return circles;
}

/*
 * step2：利用大致位置生成圆环
 * step3：内外圆环之间每隔一定角度生成扫面线
 * step4：计算边缘点坐标
 * step5：拟合圆
 */
void STEP_2345(vector<Vec3f> circles)
{
    /* 循环一层：遍历圆 */
    for (int cir = 0; cir < circles.size(); ++cir)
    {
        float* _Y = new float[LINEAMOUNT], * _P = new float[3 * LINEAMOUNT], a, b, c;
        Mat Y(LINEAMOUNT, 1, CV_32FC1, _Y), P(LINEAMOUNT, 3, CV_32FC1, _P), X, temp;
        float EdgePoint[LINEAMOUNT][2] = { 0 };   //圆上各边缘点坐标
        /* 循环二层：遍历扫面线 */
        for (int line = 0; line < LINEAMOUNT; ++line)
        {
            int LPNum = cvRound(circles[cir][2] / 4);     //每条扫描线上采样点个数
            int* LinePoint = new int[LPNum];               //用于储存每条扫面线上的所有采样点灰度值
            float theta = 2 * Pi * line / LINEAMOUNT;      //该扫面线对应的角度
            /* 循环三层：遍历线上的各点 */
            for (int dir = 0; dir < LPNum; ++dir)
            {
                /* 采样点本点的坐标 */
                float PointCol = circles[cir][0] + (circles[cir][2] - LPNum / 2 + dir) * cos(theta);
                float PointRow = circles[cir][1] - (circles[cir][2] - LPNum / 2 + dir) * sin(theta);

                /* 与扫描线垂直的线上所有点的和 */
                float sum = 0;
                int count = 0;
                /* 循环四层：对过采样点且垂直于扫描线的线上的点求平均值 */
                for (int i = 0; i < 5; ++i)
                {
                    float col = PointCol + (-5 / 2 + i) * sin(theta);
                    float row = PointRow + (-5 / 2 + i) * cos(theta);
                    if (col > 1280 || col < 0 || row>1024 || row < 0)
                        continue;
                    count++;
                    sum += BilinearInterpolation(row, col);

                }
                LinePoint[dir] = sum / count;

            }

            /* 第四步：计算各边沿点的坐标 */
            float* differ = new float[LPNum];   //微分
            float MaxDiffer = 0;                //寻最大微分的标志量

            ///* 寻找微分绝对值的最大值 */
            for (int i = 0; i < LPNum; i++)
            {
                if (i < LPNum - 1 && i > 0)
                {
                    differ[i] = abs(LinePoint[i + 1] - LinePoint[i - 1]) / 2;
                }
                else //处理扫描线边界点，由于正常情况下该点绝不可能成为边缘点之一，故干脆赋为0
                {
                    differ[i] = 0;
                }
                if (differ[i] > MaxDiffer)
                {
                    MaxDiffer = differ[i];
                    EdgePoint[line][0] = circles[cir][0] + (circles[cir][2] - LPNum / 2 + i) * cos(theta);
                    EdgePoint[line][1] = circles[cir][1] - (circles[cir][2] - LPNum / 2 + i) * sin(theta);

                }
            }
            Point c(EdgePoint[line][0], EdgePoint[line][1]);
            circle(Image, c, 3, Scalar(255, 0, 0), -1);
            delete[] differ, LinePoint;
        }



        /* 最小二乘法拟合圆 */

        for (int i = 0; i < LINEAMOUNT; ++i)
        {
            _Y[i] = -EdgePoint[i][0] * EdgePoint[i][0] - EdgePoint[i][1] * EdgePoint[i][1];
            _P[3 * i] = EdgePoint[i][0];
            _P[3 * i + 1] = EdgePoint[i][1];
            _P[3 * i + 2] = 1;
        }

        invert(P.t() * P, temp);
        X = temp * P.t() * Y;
        a = X.ptr<float>(0)[0];
        b = X.ptr<float>(1)[0];
        c = X.ptr<float>(2)[0];
        (-a / 2, -b / 2, sqrtf(a * a + b * b - 4 * a * b) / 2);

        Point center(-a / 2, -b / 2);
        circle(Image, center, sqrtf(a * a + b * b - 4 * c) / 2, Scalar(255, 0, 0), 1, 16);
        delete[] _Y, _P;

        cout << "第" << cir << "个圆的圆心是：" << "(" << -a / 2 << "," << -b / 2 << ")" << "    " << "半径是" << sqrtf(a * a + b * b - 4 * c) / 2 << endl;
    }
}




/*
 * 双线性插值得亚像素点灰度值
 */
float BilinearInterpolation(float row, float col)
{
    int x = (int)col, y = (int)row, step = srcImage.step;

    float fR1 = (x + 1 - col) * srcImage.data[y * step + x] + (col - x) * srcImage.data[step * y + x + 1];
    float fR2 = (x + 1 - col) * srcImage.data[(y + 1) * step + x] + (col - x) * srcImage.data[(y + 1) * step + x + 1];

    return (y + 1 - row) * fR1 + (row - y) * fR2;
}