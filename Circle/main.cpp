#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;
void myMeanFilter3x3(Mat src);
void myMeanFilter5x5(Mat src);
void myMeanFilterDivi(Mat src);

int main(void)
{
	double start = static_cast<double>(getTickCount());
	Mat srcImage = imread("C:/Users/fanss/Desktop/lena.jpg");
	imshow("origin", srcImage);
	//myMeanFilter3x3(srcImage);
	//myMeanFilter5x5(srcImage);
	myMeanFilterDivi(srcImage);
	double time = ((double)getTickCount() - start) / getTickFrequency();
	cout << "所用时间:" << time << "us" << endl;
	waitKey(0);
	return 0;
}

void myMeanFilter3x3(Mat src)
{
	Mat dst3 = src;
	for (int i = 1; i < src.rows-1; ++i)
	{
		for (int j = 1; j < src.cols-1; ++j) 
		{
			dst3.at<Vec3b>(i, j)[0] = (src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i - 1, j - 1)[0] + src.at<Vec3b>(i - 1, j)[0] + src.at<Vec3b>(i, j - 1)[0] +
				src.at<Vec3b>(i - 1, j + 1)[0] + src.at<Vec3b>(i + 1, j - 1)[0] + src.at<Vec3b>(i + 1, j + 1)[0] + src.at<Vec3b>(i, j + 1)[0] +
				src.at<Vec3b>(i + 1, j)[0]) / 9;
			dst3.at<Vec3b>(i, j)[1] = (src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i - 1, j - 1)[1] + src.at<Vec3b>(i - 1, j)[1] + src.at<Vec3b>(i, j - 1)[1] +
				src.at<Vec3b>(i - 1, j + 1)[1] + src.at<Vec3b>(i + 1, j - 1)[1] + src.at<Vec3b>(i + 1, j + 1)[1] + src.at<Vec3b>(i, j + 1)[1] +
				src.at<Vec3b>(i + 1, j)[1]) / 9;
			dst3.at<Vec3b>(i, j)[2] = (src.at<Vec3b>(i, j)[2] + src.at<Vec3b>(i - 1, j - 1)[2] + src.at<Vec3b>(i - 1, j)[2] + src.at<Vec3b>(i, j - 1)[2] +
				src.at<Vec3b>(i - 1, j + 1)[2] + src.at<Vec3b>(i + 1, j - 1)[2] + src.at<Vec3b>(i + 1, j + 1)[2] + src.at<Vec3b>(i, j + 1)[2] +
				src.at<Vec3b>(i + 1, j)[2]) / 9;
				
		}
	}
	imshow("blur3x3", dst3);
}

void myMeanFilter5x5(Mat src)
{
	Mat dst5 = src;
	for (int i = 2; i < src.rows - 2; ++i)
	{
		for (int j = 2; j < src.cols - 2; ++j)
		{
			dst5.at<Vec3b>(i, j)[0] = (src.at<Vec3b>(i - 2, j - 2)[0] + src.at<Vec3b>(i - 2, j - 1)[0] + src.at<Vec3b>(i - 2, j)[0] + src.at<Vec3b>(i - 2, j + 1)[0] + src.at<Vec3b>(i - 2, j + 2)[0] +
				src.at<Vec3b>(i - 1, j - 2)[0] + src.at<Vec3b>(i - 1, j - 1)[0] + src.at<Vec3b>(i - 1, j)[0] + src.at<Vec3b>(i - 1, j + 1)[0] + src.at<Vec3b>(i - 1, j + 2)[0] +
				src.at<Vec3b>(i, j - 2)[0] + src.at<Vec3b>(i, j - 1)[0] + src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j + 1)[0] + src.at<Vec3b>(i, j + 2)[0] +
				src.at<Vec3b>(i + 1, j - 2)[0] + src.at<Vec3b>(i + 1, j - 1)[0] + src.at<Vec3b>(i + 1, j)[0] + src.at<Vec3b>(i + 1, j + 1)[0] + src.at<Vec3b>(i + 1, j + 2)[0] +
				src.at<Vec3b>(i + 2, j - 2)[0] + src.at<Vec3b>(i + 2, j - 1)[0] + src.at<Vec3b>(i + 2, j)[0] + src.at<Vec3b>(i + 2, j + 1)[0] + src.at<Vec3b>(i + 2, j + 2)[0]) / 25;
			dst5.at<Vec3b>(i, j)[1] = (src.at<Vec3b>(i - 2, j - 2)[1] + src.at<Vec3b>(i - 2, j - 1)[1] + src.at<Vec3b>(i - 2, j)[1] + src.at<Vec3b>(i - 2, j + 1)[1] + src.at<Vec3b>(i - 2, j + 2)[1] +
				src.at<Vec3b>(i - 1, j - 2)[1] + src.at<Vec3b>(i - 1, j - 1)[1] + src.at<Vec3b>(i - 1, j)[1] + src.at<Vec3b>(i - 1, j + 1)[1] + src.at<Vec3b>(i - 1, j + 2)[1] +
				src.at<Vec3b>(i, j - 2)[1] + src.at<Vec3b>(i, j - 1)[1] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j + 1)[1] + src.at<Vec3b>(i, j + 2)[1] +
				src.at<Vec3b>(i + 1, j - 2)[1] + src.at<Vec3b>(i + 1, j - 1)[1] + src.at<Vec3b>(i + 1, j)[1] + src.at<Vec3b>(i + 1, j + 1)[1] + src.at<Vec3b>(i + 1, j + 2)[1] +
				src.at<Vec3b>(i + 2, j - 2)[1] + src.at<Vec3b>(i + 2, j - 1)[1] + src.at<Vec3b>(i + 2, j)[1] + src.at<Vec3b>(i + 2, j + 1)[1] + src.at<Vec3b>(i + 2, j + 2)[1]) / 25;
			dst5.at<Vec3b>(i, j)[2] = (src.at<Vec3b>(i - 2, j - 2)[2] + src.at<Vec3b>(i - 2, j - 1)[2] + src.at<Vec3b>(i - 2, j)[2] + src.at<Vec3b>(i - 2, j + 1)[2] + src.at<Vec3b>(i - 2, j + 2)[2] +
				src.at<Vec3b>(i - 1, j - 2)[2] + src.at<Vec3b>(i - 1, j - 1)[2] + src.at<Vec3b>(i - 1, j)[2] + src.at<Vec3b>(i - 1, j + 1)[2] + src.at<Vec3b>(i - 1, j + 2)[2] +
				src.at<Vec3b>(i, j - 2)[2] + src.at<Vec3b>(i, j - 1)[2] + src.at<Vec3b>(i, j)[2] + src.at<Vec3b>(i, j + 1)[2] + src.at<Vec3b>(i, j + 2)[2] +
				src.at<Vec3b>(i + 1, j - 2)[2] + src.at<Vec3b>(i + 1, j - 1)[2] + src.at<Vec3b>(i + 1, j)[2] + src.at<Vec3b>(i + 1, j + 1)[2] + src.at<Vec3b>(i + 1, j + 2)[2] +
				src.at<Vec3b>(i + 2, j - 2)[2] + src.at<Vec3b>(i + 2, j - 1)[2] + src.at<Vec3b>(i + 2, j)[2] + src.at<Vec3b>(i + 2, j + 1)[2] + src.at<Vec3b>(i + 2, j + 2)[2]) / 25;

		}
	}
	imshow("blur5x5", dst5);
}

void myMeanFilterDivi(Mat src)
{
	Mat dstd = src;
	int sum3row_0[1000], sum3row_1[1000], sum3row_2[1000];//列和
	for (int i = 1; i < src.rows-1; i++)
	{
		for (int k = 0; k < src.cols; k++)
		{
			sum3row_0[k] = src.at<Vec3b>(i - 1, k)[0]+ src.at<Vec3b>(i , k)[0]+ src.at<Vec3b>(i + 1, k)[0];
			sum3row_1[k] = src.at<Vec3b>(i - 1, k)[1] + src.at<Vec3b>(i, k)[1] + src.at<Vec3b>(i + 1, k)[1];
			sum3row_2[k] = src.at<Vec3b>(i - 1, k)[2] + src.at<Vec3b>(i, k)[2] + src.at<Vec3b>(i + 1, k)[2];
		}

		for (int j = 1; j < src.cols-1; j++)
		{
			dstd.at<Vec3b>(i, j)[0] = (sum3row_0[j - 1] + sum3row_0[j] + sum3row_0[j + 1]) / 9;
			dstd.at<Vec3b>(i, j)[1] = (sum3row_1[j - 1] + sum3row_1[j] + sum3row_1[j + 1]) / 9;
			dstd.at<Vec3b>(i, j)[2] = (sum3row_2[j - 1] + sum3row_2[j] + sum3row_2[j + 1]) / 9;
		}
	}
	imshow("blurDivi", dstd);
}