#pragma once
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class CEyeDetector
{
private:
	CascadeClassifier eyeDetector;	//�۾������
    CascadeClassifier leftEyeDetector;//���ۼ����
    CascadeClassifier rightEyeDetector;//���ۼ����

public:

    CEyeDetector(void);
	~CEyeDetector(void);


    /**
    �����������ͼ���м����۾���λ�ã�����������ļ���������ⵥֻ�۾��ģ����ص�vector����ֻ��һ��Ԫ��
    @param	face	����ͼ��
    @return	�����м�⵽���۾����ص�vector�����У����û�м�⵽��vector�Ĵ�СΪ0
	*/	
	vector<Rect> detectEyes(Mat face);

    /**
    �������λ��
    @param	face	����ͼ��
    @return	����λ�ã����û��⵽����Rect(0)
    */
    Rect detectLeftEye(Mat face);

    /**
    �������λ��
    @param	face	����ͼ��
    @return	����λ�ã����û��⵽����Rect(0)
    */
    Rect detectRightEye(Mat face);

private:
    /**
     * @brief Ԥ����ͼ��
     * @param img   ����ͼ����ԭͼ�ϲ���
     */
    void preprocessImage(Mat img);
};

