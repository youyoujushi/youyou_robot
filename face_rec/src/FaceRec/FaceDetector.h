#pragma once
#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "EyeDetector.h"
#include "global.h"


using namespace std;
using namespace cv;

class CFaceDetector
{

private:
	CascadeClassifier	faceDetector;	//���������
	CEyeDetector		eyeDetector;	//�۾������
//	vector<Mat>			gatheredFaces;	//�洢�ɼ�������ͼ��

public:

	/**
	������⹹����
    @param	faceClassiferFilePath �����������ļ�·��,
	*/
    CFaceDetector(char* faceClassiferFilePath);
	~CFaceDetector(void);

	/**
	�������ͼ���м������е�����
	@param	srcImg	������ͼ��
    @return	�����м�⵽������λ�÷��ص�vector�����У����û�м�⵽������vector�Ĵ�СΪ��
	*/	
	vector<Rect> detectFaces(Mat srcImg);

    /**
    �������ͼ���м������е�������������������Ԥ����������ͼƬ����ֱ�Ӵ���ʶ��ģ��ʶ��
    @param	srcImg              ������ͼ��
    @param  preprocessedFaces   �洢Ԥ����������ͼ��
    @param  desireFaceSize      ������������С
    @return	�����м�⵽������λ�÷��ص�vector�����У����û�м�⵽������vector�Ĵ�СΪ��
    */
    vector<Rect> detectFacesAndPreprocess(Mat srcImg,vector<Mat>& preprocessedFaces,
                                          Size desireFaceSize=Size(92,112));

    /**
     * @brief ��ͼ���ļ��м������
     * @param path      �ļ�Ŀ¼����Ŀ¼�������ļ�����Ϊͬһ��
     * @param desireFaceSize    ����������ߴ�
     * @return
     */
    vector<Mat> detectFacesAndPreprocessFromFiles(String path,Size desireFaceSize=Size(92,112));

private:

    /**
    Ԥ�����ʶ������ͼ��ͼ��ļ��α任����Ȩֱ��ͼ���⻯�����롢ȥ�����������أ�
    @param	face	���������ͼ����ԭͼ����
    @param  desireFaceSize    ������������С
    @return ����Ԥ��������true of false
    */
    bool preprocessFace(Mat& face,Size desireFaceSize);

    /**
    �������α任�����š���ת��ƽ�ƣ���ʹ�ô����ͼ����ӽ���׼����������
    @param	face        ���������ͼ����ԭͼ����������Ϊ��ͨ���Ҷ�ͼ
    @param  leftEye     ����λ��
    @param  rightEye    ����λ��
    @param  desireFaceSize    ������������С
    @return	�任�ɹ�����true��ʧ�ܷ���false
    */
    bool faceGeometricTransfer(Mat& face,Rect leftEye,Rect rightEye,Size desireFaceSize);

    /**
    ֱ��ͼ���⻯���ȶ����������ͼ������⻯���ٶ�����ͼ����⻯����󣬰�����ͼ����ͬ��Ȩ�ػ����һ��
        �����������̶Ƚ��͹��ն�ͼƬ��Ӱ��
    @param	face	���������ͼ����ԭͼ����������Ϊ��ͨ���Ҷ�ͼ
    */
    void equalizeFace(Mat& face);

    /**
    �����������ͼ�����һ����Բ���ɰ棬���ε�����һЩ���õ�����
    @param	face	���������ͼ����ԭͼ����
    @param  desireFaceSize    ������������С
    */
    void maskFace(Mat& face,Size desireFaceSize);

	/**
	����������ͼƬ
	@param	srcImg		ԴͼƬ
	@param	destWidth	Ŀ��ͼƬ��ȣ����ű���Ϊ��srcImg.size().width/destImg.size().wdith,
							�����ֵ����0������destHeight����
	@param	destHeight	Ŀ��ͼƬ�߶ȣ����ű���Ϊ��srcImg.size().height/destImg.size().height,
							���destWidthС�ڵ���0��ʹ�øò���
	@return	���ش��ڵ���0����ֵΪ���ű�������������ʧ��		
	*/
	float scaleImg(Mat& srcImg,int destWidth,int destHeight=0);

	
	
};

