#ifndef CRECOGNIZERESULT_H
#define CRECOGNIZERESULT_H

#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

enum EN_REC_RESULT_STATUS{
    REC_SUCCESS,        //ʶ��ɹ�
    REC_TIMEOUT,        //ʶ��ʱ
    REC_UNKNOWN_BODY,   //δ֪��ʶ�����
    REC_NOBODY          //δ��⵽����
};

//ʶ�����������Ϣ
struct ST_REC_RESULT{
    int             label;          //ʶ��ģ���У�������Ӧ�ı�ǩ
    double          confidence;     //ʶ�������Ŷ�
    char            sex;
    unsigned short  age;
    string          name;

    Rect            faceRect;
    Rect            leftEyeRect;
    Rect            rightEyeRect;
    Rect            noseRect;
    Rect            mouseRect;

};

/**
 * @brief ����ʶ����
 */
class CRecognizeResult
{
public:
    EN_REC_RESULT_STATUS    status;     //ʶ����״̬
    vector<ST_REC_RESULT>   results;    //����ʶ����Ķ�����Ϣ

public:
    CRecognizeResult();
};

#endif // CRECOGNIZERESULT_H
