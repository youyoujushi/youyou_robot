#ifndef _GLOBAL
#define _GLOBAL

#include "YYJSLogger.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include <unistd.h>
#include <actionlib/server/simple_action_server.h>
#include <face_rec/face_recAction.h>

using namespace cv;
using namespace std;

typedef actionlib::SimpleActionServer<face_rec::face_recAction> FaceRecServer;

//#define _WINDOWS
#define _LINUX

/************ȫ�ֱ���****************/
extern CYYJSLogger logger;
extern FaceRecServer *face_rec_server;
extern FaceRecServer::Result face_rec_result; //ʶ��action��������
extern bool face_rec_completed; //ʶ�������Ƿ����

/************ȫ�ֳ���****************/
extern string faceDataPath;

/************��****************/
#define IS_ZERO_RECT(r) ((r).size().width == 0 || (r).size().height == 0)   //�Ƿ��ǿվ���

/****************ȫ�ֺ���*****************/
/**
*	��Դͼת��Ϊ�Ҷ�ͼ
*	@param	srcImg	Դͼ
*/
void convertToGrayImg(Mat& srcImg);

/**
 * @brief �ļ���Ŀ¼�Ƿ����
 * @param   path    ·��
 * @return  ���ڷ���true�������ڷ���false
 */
bool isFileOrDicExist(const char *path);

/**
 * @brief ����Ŀ¼
 * @param path
 * @return  �ɹ�����true�����򷵻�false
 */
bool createDic(const char *path);

/**
 * @brief ����Ŀ¼�������ļ�
 * @param path  Ŀ¼·��
 * @param files ����Ŀ¼�������ļ���ȫ·����
 */
void listDir(const char *path,vector<String>& files);

#endif
