#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/face.hpp"

using namespace std;
using namespace cv::face;
using namespace cv;


//����ʶ����ö������
enum EN_FACE_REC_TYPE{
    EIGEN,
    FISHER,
    LBPH
};




class CFaceRecognizer
{
private:
	Mat		curFace;
	Size	faceSize;	//����ͼ���С
	Ptr<BasicFaceRecognizer>	eigenFaceRec;		//����������ʶ����
	Ptr<BasicFaceRecognizer>	fisherFaceRec;		//�����б��������ʶ����
	Ptr<LBPHFaceRecognizer>		LBPHFaceRec;		//LBPH����ʶ����
    bool    trained;    //�Ƿ���ѵ����ģ��
    EN_FACE_REC_TYPE            rec_type;           //��ǰʹ�õ�����ʶ��������
//    ST_FACE_REC_MODEL           recModel;           //ģ����Ϣ
	/**
	faceDataPathĿ¼�ṹ
	.
	models.yaml			--�������ѵ��ģ��
	faces.csv			--�������������ļ� 
	data				--��������Ŀ¼
		Ŀ¼��1				--	��Ŀ¼���ļ�Ϊͬһ�˵�����ͼ��
			0.jpg				--����ͼ��
			1.jpg
			...
		Ŀ¼��n
	*/
    string	faceDataPath;	//����ͼ���ļ�·��,

public:
    /**
    ������
    @param	faceDataPath	��������·��
    @param	type            ����ʶ��������
    */
    CFaceRecognizer(string faceDataPath,EN_FACE_REC_TYPE type);
	~CFaceRecognizer(void);

	/**
    ѵ��ʶ��ģ�ͣ�ÿ�ε��ø÷������������֮ǰѵ����ģ������
    @param	faces	����ͼ��
    @param	labels  ͼ���Ӧ�ı�ǩ
	*/
    void train(vector<Mat> faces,vector<int> labels);

	/**
	����ʶ��ģ�ͣ�faces��labelsһһ��Ӧ��ֻ��LBPHʶ�����ſ��Ը���ģ��
	@param	faces	ѵ����ͼ��
	@param	labels	ͼ���Ӧ�ı�ǩ
	*/
    void updateModel(vector<Mat> faces,vector<int> labels);

    /**
    ����ʶ��ģ�ͣ�����faces����ͬһ��label��ֻ��LBPHʶ�����ſ��Ը���ģ��
    @param	faces	ѵ����ͼ��
    @param	label	ͼ���Ӧ�ı�ǩ
    */
    void updateModel(vector<Mat> faces,int label);

	/**
	����ʶ��
	@param	face		��ʶ������
	@param	confidence	ʶ�������Ŷ�
	@param	label		ʶ�����������Ӧ�ı�ǩ
	*/
	void recognize(Mat face,double& confidence,int& label);
	
	/**
	������ѵ����ģ������
	*/
    bool loadTrainedModel();

	/**
	������ѵ����ģ������
	*/
	void saveTrainedModel();

//    /**
//    ����������ǩ��������
//    @param  label   ������ǩ
//    */
//    string getNameFromLabel(int label);

private:

};

