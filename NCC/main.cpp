#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;

void tempProcess(Mat temp);
void NCC(Mat tempImage, Mat tarImage,int rotateAngle);
void rotate_arbitrarily_angle(Mat& src, Mat& dst, float angle);

float tempdata[2];   //第一个元素表示模板的mt,第二个元素表示模板的st^2
float maxNCC = 0, maxRow = 0, maxCol = 0;
int angle = 0;

int main(void)
{
	Mat tempImage, tarImage, tarImage1, tarImage2, tarImage3, tarImage4, tempImage1, tempImage2,tempImage3, tempImage4;

	//读图
	Mat temp = imread("C:/Users/fanss/Desktop/NCCImage/pattern.bmp");
	Mat tar = imread("C:/Users/fanss/Desktop/NCCImage/IMAGEB21.bmp");
	//转为灰度图
	cvtColor(temp, tempImage, COLOR_BGR2GRAY);
	cvtColor(tar, tarImage, COLOR_BGR2GRAY);
	

	pyrDown(tarImage, tarImage1, Size(tarImage.cols / 2, tarImage.rows / 2));
	pyrDown(tarImage1, tarImage2, Size(tarImage1.cols / 2, tarImage1.rows / 2));
	pyrDown(tarImage2, tarImage3, Size(tarImage2.cols / 2, tarImage2.rows / 2));
	pyrDown(tarImage3, tarImage4, Size(tarImage3.cols / 2, tarImage3.rows / 2));

	pyrDown(tempImage, tempImage1, Size(tempImage.cols / 2, tempImage.rows / 2));
	pyrDown(tempImage1, tempImage2, Size(tempImage1.cols / 2, tempImage1.rows / 2));
	pyrDown(tempImage2, tempImage3, Size(tempImage2.cols / 2, tempImage2.rows / 2));
	pyrDown(tempImage3, tempImage4, Size(tempImage3.cols / 2, tempImage3.rows / 2));

	//计算模板的灰度均值和均方差
	Mat tempRotate;
	double start = static_cast<double>(getTickCount());
	
	for (int i = 0; i < 360; i++)
	{
		rotate_arbitrarily_angle(tempImage4, tempRotate, i);
		tempProcess(tempRotate);
		NCC(tempRotate, tarImage4,i);
		cout << "正在处理的角度" << i << endl;
	}


	cout << "最大的NCC" << maxNCC << endl;
	cout << "最大的坐标" << "  " << "(" << maxCol * 16 << "," << maxRow * 16 << ")" << endl;
	cout << "转角" << angle << endl;

	double time = ((double)getTickCount() - start) / getTickFrequency();
	cout << "所用时间:" << time << "s" << endl;
	rectangle(tarImage, Rect(maxCol*16, maxRow*16, tempImage.cols, tempImage.rows),Scalar(255,0,0));
	imshow("1", tarImage);

	waitKey(0);
	return 0;
}

/* 该函数作用是计算出模板的mt和st^2 */
void tempProcess(Mat temp)
{
	int n = temp.cols * temp.rows;   //模板像素总数
	//求模板灰度值均值
	float mt = 0;
	for (int r = 0; r < temp.rows; r++)
	{
		for (int c = 0; c < temp.cols; c++)
		{
			mt += temp.at<uchar>(r, c);
		}
	}
	mt /= n;

	//求模板灰度值均方差
	float st2 = 0;
	for (int r = 0; r < temp.rows; r++)
	{
		for (int c = 0; c < temp.cols; c++)
		{
			st2 += pow((temp.at<uchar>(r, c) - mt), 2);
		}
	}
	st2 /= n;
	tempdata[0] = mt;
	tempdata[1] = st2;
}
/* 该函数的作用是求取待检测图像相对于模板的NCC值 */
void NCC(Mat tempImage,Mat tarImage,int rotateAngle)
{
	
	for (int r = 0; r <= tarImage.rows - tempImage.rows; r++)
	{
		for (int c = 0; c <= tarImage.cols - tempImage.cols; c++)
		{
			float NCC = 0, mf = 0, sf2 = 0;

			//计算(r,c)的mf
			for (int u = 0; u < tempImage.rows; u++)
			{
				for (int v = 0; v < tempImage.cols; v++)
				{
					mf += tarImage.at<uchar>(r + u, c + v);
				}
			}
			mf /= tempImage.rows * tempImage.cols;

			//计算(r,c)的sf2
			for (int u = 0; u < tempImage.rows; u++)
			{
				for (int v = 0; v < tempImage.cols; v++)
				{
					sf2 += pow((tarImage.at<uchar>(r + u, c + v)-mf),2);
				}
			}
			sf2 /= tempImage.rows * tempImage.cols;
			float sf = sqrt(sf2);

			//计算(r,c)的NCC
			for (int u = 0; u < tempImage.rows; u++)
			{
				for (int v = 0; v < tempImage.cols;v++)
				{
					NCC += ((tempImage.at<uchar>(u, v) - tempdata[0]) / sqrt(tempdata[1])) * ((tarImage.at<uchar>(u + r, v + c) - mf) / sf);
				}
			}
			NCC /= tempImage.rows * tempImage.cols;

			if (NCC > maxNCC)
			{
				maxNCC = NCC;
				maxRow = r;
				maxCol = c;
				angle = rotateAngle;
			}
			//cout<<"正在处理的坐标"<< "  " << "(" << r << "," << c << ")" << endl;
		}
	}

	
}

/* 该函数的作用是旋转图像 */
void rotate_arbitrarily_angle(Mat& src, Mat& dst, float angle)
{
	float radian = (float)(angle / 180.0 * CV_PI);

	//填充图像
	int maxBorder = (int)(max(src.cols, src.rows) * 1.414); //即为sqrt(2)*max
	int dx = (maxBorder - src.cols) / 2;
	int dy = (maxBorder - src.rows) / 2;
	copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

	//旋转
	Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
	Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
	warpAffine(dst, dst, affine_matrix, dst.size());

	//计算图像旋转之后包含图像的最大的矩形
	float sinVal = abs(sin(radian));
	float cosVal = abs(cos(radian));
	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//剪掉多余边框
	int x = (dst.cols - targetSize.width) / 2;
	int y = (dst.rows - targetSize.height) / 2;
	Rect rect(x, y, targetSize.width, targetSize.height);
	dst = Mat(dst, rect);
}