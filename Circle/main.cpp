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
 * step1��ͼ��ָ�õ����µ�Բ�ĺͰ뾶
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
        //����Բ��
        circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        //����Բ����
        circle(srcImage, center, radius, Scalar(255, 50, 0), 3, 8, 0);
    }
    imshow("�ָ���", srcImage);
    return circles;
}

/*
 * step2�����ô���λ������Բ��
 * step3������Բ��֮��ÿ��һ���Ƕ�����ɨ����
 * step4�������Ե������
 * step5�����Բ
 */
void STEP_2345(vector<Vec3f> circles)
{
    /* ѭ��һ�㣺����Բ */
    for (int cir = 0; cir < circles.size(); ++cir)
    {
        float* _Y = new float[LINEAMOUNT], * _P = new float[3 * LINEAMOUNT], a, b, c;
        Mat Y(LINEAMOUNT, 1, CV_32FC1, _Y), P(LINEAMOUNT, 3, CV_32FC1, _P), X, temp;
        float EdgePoint[LINEAMOUNT][2] = { 0 };   //Բ�ϸ���Ե������
        /* ѭ�����㣺����ɨ���� */
        for (int line = 0; line < LINEAMOUNT; ++line)
        {
            int LPNum = cvRound(circles[cir][2] / 4);     //ÿ��ɨ�����ϲ��������
            int* LinePoint = new int[LPNum];               //���ڴ���ÿ��ɨ�����ϵ����в�����Ҷ�ֵ
            float theta = 2 * Pi * line / LINEAMOUNT;      //��ɨ���߶�Ӧ�ĽǶ�
            /* ѭ�����㣺�������ϵĸ��� */
            for (int dir = 0; dir < LPNum; ++dir)
            {
                /* �����㱾������� */
                float PointCol = circles[cir][0] + (circles[cir][2] - LPNum / 2 + dir) * cos(theta);
                float PointRow = circles[cir][1] - (circles[cir][2] - LPNum / 2 + dir) * sin(theta);

                /* ��ɨ���ߴ�ֱ���������е�ĺ� */
                float sum = 0;
                int count = 0;
                /* ѭ���Ĳ㣺�Թ��������Ҵ�ֱ��ɨ���ߵ����ϵĵ���ƽ��ֵ */
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

            /* ���Ĳ�����������ص������ */
            float* differ = new float[LPNum];   //΢��
            float MaxDiffer = 0;                //Ѱ���΢�ֵı�־��

            ///* Ѱ��΢�־���ֵ�����ֵ */
            for (int i = 0; i < LPNum; i++)
            {
                if (i < LPNum - 1 && i > 0)
                {
                    differ[i] = abs(LinePoint[i + 1] - LinePoint[i - 1]) / 2;
                }
                else //����ɨ���߽߱�㣬������������¸õ�������ܳ�Ϊ��Ե��֮һ���ʸɴำΪ0
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



        /* ��С���˷����Բ */

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

        cout << "��" << cir << "��Բ��Բ���ǣ�" << "(" << -a / 2 << "," << -b / 2 << ")" << "    " << "�뾶��" << sqrtf(a * a + b * b - 4 * c) / 2 << endl;
    }
}




/*
 * ˫���Բ�ֵ�������ص�Ҷ�ֵ
 */
float BilinearInterpolation(float row, float col)
{
    int x = (int)col, y = (int)row, step = srcImage.step;

    float fR1 = (x + 1 - col) * srcImage.data[y * step + x] + (col - x) * srcImage.data[step * y + x + 1];
    float fR2 = (x + 1 - col) * srcImage.data[(y + 1) * step + x] + (col - x) * srcImage.data[(y + 1) * step + x + 1];

    return (y + 1 - row) * fR1 + (row - y) * fR2;
}