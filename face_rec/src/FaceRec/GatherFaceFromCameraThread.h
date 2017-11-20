#pragma once

//#define __cplusplus 201103L
//#define _GLIBCXX_HAS_GTHREADS
//#define _GLIBCXX_USE_C99_STDINT_TR1
#include "opencv2/opencv.hpp"
#include <thread>
#include "FaceDetector.h"
#include "RecognizeResult.h"

using namespace std;
using namespace cv;

/**
������ͷ�ɼ�����ͼƬ���̣߳����಻���̰߳�ȫ�ģ�������Ϊȫ��Ωһʵ��
*/
class CFaceWorkThread
{
private:
	int				gatherTimeInterval;	//�ɼ�ʱ��������λΪ����
	thread			*gatherThread;		//�ɼ��̶߳���
	bool			b_exitThread;		//�Ƿ��˳��̵߳ı�־
	vector<Mat>		gatheredFaces;		//�ɼ�������ͼ��
	vector<Rect>	faceRectsInImg;		//������Դͼ�е�λ�úͳߴ�
	Size			faceSize;			//�ɼ������������С
    char*           faceDataPath;       //����ͼ������·��
    CRecognizeResult recResult;         //����ʶ����

public:
	CFaceDetector	faceDetector;	//����ʶ����
	bool			isGathering;	//�Ƿ����ڲɼ�����
	

public:
	/**
	������
	@param	faceClassiferFilePath �����������ļ�·��
    @param  faceDataPath          ��������·��
	*/
    CFaceWorkThread(char* faceClassiferFilePath,char *faceDataPath);
    ~CFaceWorkThread(void);

	void operator()();//�̺߳���

	/**
	��ʼ������ͷ�ɼ�����ͼ�񣬲ɼ���ͼ�񱣴����ڴ��С�
	�ɼ�������������һ��ÿ��һ��ʱ��Ųɼ�һ�Σ������ɼ�������ͼ����ϴ�����������Ųɼ�
	@param	timeInterval	�ɼ�����ͼ���ʱ��������λΪ���룬Ĭ��ÿ��ɼ�һ��
	@param	faceSize		�ɼ����������ͼƬ��С����Ҫʱ���ʶ�������������ת��
	*	
	*/	
    void beginGatherFaceFromCamera(Size faceSize = Size(92,112), int timeInterval=1000);

    /**
    ֹͣ������ͷ�ɼ�����ͼ��
    @return	���زɼ���������ͼ�������
    */
    int endGatherFaceFromCamera();

	/**
    ����ɼ���������ͼ���ļ�
    @param	name		����
	@param	id			����ͼ���id��ʶ
	*/
    void saveGatheredFacesToFile(char *name,int id);
	
    // /**
    // ��ʼ������ͷ�ɼ�����ͼ�񣬲ɼ���ͼ�񱣴����ڴ��С�
    // �ɼ�������������һ��ÿ��һ��ʱ��Ųɼ�һ�Σ������ɼ�������ͼ����ϴ�����������Ųɼ�
    // @param	timeInterval	�ɼ�����ͼ���ʱ��������λΪ���룬Ĭ��ÿ��ɼ�һ��
    // @param	faceSize		�ɼ����������ͼƬ��С����Ҫʱ���ʶ�������������ת��
    // *
    // */
    // void beginGatherFaceFromCamera(Size faceSize = Size(92,112), int timeInterval=1000);

	/**
	��������ͼƬ�����ƶ�
	@param	img1	ͼƬ1
	@param	img2	ͼƬ2
	@return	�������ƶȣ�Խ�ӽ�0Խ����,0.2ͼ��û���ƶ���0.4ͼ�����ƶ���0.3����ֵ		
	*/
	double calSimilarity(Mat img1,Mat img2);

	vector<Mat> getGatheredFaces();

private:
    /**
    ��ȡ�µ�����ͼ���ǩ
    */
    int getNewFaceLabel();
};
