#pragma once

//#define __cplusplus 201103L
//#define _GLIBCXX_HAS_GTHREADS
//#define _GLIBCXX_USE_C99_STDINT_TR1
#include "opencv2/opencv.hpp"
#include <thread>
#include "FaceDetector.h"
#include "RecognizeResult.h"
#include "FaceRecognizer.h"
#include <map>

using namespace std;
using namespace cv;

enum EN_WORK_THREAD_STATUS{
    IDLE,           //����
    GATHER_FROM_CAMERA,      //���ڴ�����ͷ�ɼ�����
    GATHER_FROM_FILES,      //���ڴ��ļ��ɼ�����
    RECOGNIZING_ONCE,		//ʶ��������ֻʶ��һ��
	RECOGNIZING_PERIOD,    //����ʶ������һ��ʱ�䣬��ʶ�������������Ϊʶ����
};

struct ST_GATHER_FACE_INFO{
    vector<Mat>		gatheredFaces;		//�ɼ�������ͼ��
    vector<Rect>	faceRectsInImg;		//������Դͼ�е�λ�úͳߴ�
    int     label;
    string  name;
    void clearInfo(){
        gatheredFaces.clear();
        faceRectsInImg.clear();
    }
};

//����ʶ���ǩ��Ϣ
struct ST_FACE_DATA_INFO{
    vector<string>  names;      //����
    vector<int>     labels;     //����ͼ���ǩ��������һһ��Ӧ
    void clearInfo(){
        names.clear();
        labels.clear();
    }
};

/**
����ʶ�����̣߳����಻���̰߳�ȫ�ģ�������Ϊȫ��Ωһʵ��
*/
class CFaceWorkThread
{
private:
    EN_WORK_THREAD_STATUS   workStatus; //�̵߳Ĺ���״̬
	int				gatherTimeInterval;	//�ɼ�ʱ��������λΪ����
	thread			*gatherThread;		//�ɼ��̶߳���
	bool			b_exitThread;		//�Ƿ��˳��̵߳ı�־
    ST_GATHER_FACE_INFO gatheredFaceInfos;    //�ɼ�������������Ϣ
	Size			faceSize;			//�ɼ������������С
    char*           faceDataPath;       //����ͼ������·��
    CRecognizeResult recResult;         //����ʶ����
    int             faceRecTimeout;     //����ʶ��ʱʱ�䣬����Ϊ��λ
    ST_FACE_DATA_INFO faceDataInfo;   //����ʶ���ǩ��Ӧ��Ϣ
    String          gatherFilePath;     //���ļ��ɼ������Ŀ¼·��
	vector<vector<int>>	recognizeInfoPeriod;//����ʶ��Ľ������Ƕ��vector������Ԫ�أ���һ��Ϊlabel���ڶ���Ϊ��label��ʶ����Ĵ���
	int				recognizePeriodTime;	//����ʶ���ʱ�䳤�ȣ��Ժ���Ϊ��λ

public:
    CFaceDetector	faceDetector;       //���������
    CFaceRecognizer faceRecognize;      //����ʶ����

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
     * @brief ��ʼ��
     */
    void initialize();

    /**
     * @brief getWorkThreadStatus
     * @return �����̵߳Ĺ���״̬
     */
    EN_WORK_THREAD_STATUS getWorkThreadStatus();

	/**
	��ʼ������ͷ�ɼ�����ͼ�񣬲ɼ���ͼ�񱣴����ڴ��С�
	�ɼ�������������һ��ÿ��һ��ʱ��Ųɼ�һ�Σ������ɼ�������ͼ����ϴ�����������Ųɼ�
	@param	timeInterval	�ɼ�����ͼ���ʱ��������λΪ���룬Ĭ��ÿ��ɼ�һ��
	@param	faceSize		�ɼ����������ͼƬ��С����Ҫʱ���ʶ�������������ת��
	*	
	*/	
    void beginGatherFaceFromCamera(Size size, int timeInterval=800);

    /**
    ֹͣ������ͷ�ɼ�����ͼ��
    @return	���زɼ���������ͼ�������
    */
    int endGatherFaceFromCamera();

    /**
     * @brief ��ͼ���ļ��ж�ȡ����
     * @param path  �ļ�Ŀ¼����Ŀ¼���ļ���Ϊͬһ��
     */
    void gatherFaceFromFiles(String path,Size size);

    /**
     * @brief ���òɼ����������
     * @param name  ����
     */
    void setGatherName(string name);

	/**
    ����ɼ���������ͼ���ļ�
	*/
    void saveGatheredFacesToFile();
	
    /**
    ������ͷʶ������
    @param	timeOut	ʶ��ĳ�ʱʱ��
    *
    */
    void recognizeFaceFromCamera(int timeOut=5000);

	/**
    ����Ƶ�����г���ʶ��ȡʶ�������������Label��������ʶ��ĸ���
    @param	period	ʶ��ĳ���ʱ�䣬��λ����
    *
    */
    void recognizeFaceFromCameraPeriod(int period=3000);

    /**
    ��ͼ���ļ�ʶ������
    @param	filePath	�ļ�·��
    *
    */
    void recognizeFaceFromFile(string filePath);

	/**
	��������ͼƬ�����ƶ�
	@param	img1	ͼƬ1
	@param	img2	ͼƬ2
	@return	�������ƶȣ�Խ�ӽ�0Խ����,0.2ͼ��û���ƶ���0.4ͼ�����ƶ���0.3����ֵ		
	*/
	double calSimilarity(Mat img1,Mat img2);

    /**
    @return	���زɼ�������������
    */
    ST_GATHER_FACE_INFO getGatheredFacesInfo();

    /**
     * @brief ����csv�ļ����ݣ���ȡ����������Ϣ
     */
    void readFaceDataCsv(vector<Mat>& faces,vector<int>& labels,vector<string>& names);

    /**
     * @brief ���¶������������Ϣ
     * @param labels
     * @param names
     */
    void updateFaceDataInfo(vector<int> labels,vector<string> names);

    /**
     * @brief ���µ�������������Ϣ
     * @param labels
     * @param names
     */
    void updateFaceDataInfo(int label,string name);


    /**
     * @brief ��ղɼ�������
     */
    void clearGatheredFaceInfo();

    /**
     * @brief ����label���ض�Ӧ������
     * @param label
     * @return
     */
    string getNameFromLabel(int label);

private:

    /**
    ��ȡ�µ�����ͼ���ǩ
    */
    int getNewFaceLabel();



};
