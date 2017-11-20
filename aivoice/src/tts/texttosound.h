#ifndef TEXTTOSOUND_H
#define TEXTTOSOUND_H


#include <iostream>
#include <time.h>
#include<curl/curl.h>

using namespace std;

#define ZERO_TTS_PARAM(param) memset(&param,0,sizeof(param))


//tts�Ĳ���
struct TTS_PARAM{

    int rdn;           //�ϳ���Ƶ���ַ�����ʽ
    int volume;        //�ϳ���Ƶ������
    int pitch;         //�ϳ���Ƶ������
    int speed;         //�ϳ���Ƶ��Ӧ������
    int sample_rate;   //�ϳ���Ƶ������
    std::string voice_name;// �ϳɷ�����
    std::string text_encoding; //�ϳ��ı������ʽ

};

struct BAIDU_TOKEN_INFO{
    string  token;      //���ðٶ��������������token�ַ���
    time_t  expires;    //token�Ĵ�ʱ��ǰ��Ч
    string  apiKey;     //token��Ӧ��Ӧ��apiKey
    string  secretKey;  //token��Ӧ��secrectKey
};


//ָ��ʹ����һ��tts
enum TTS_TYPE{
    TTS_BAIDU,  //�ٶ�TTS
    TTS_XUNFEI  //�ƴ�Ѷ��TTS
};

class TextToSound
{

private:
    TTS_TYPE    ttsType;            //tts������
    string      ttsSoundPath;       //ttsת������ļ�·����
    CURL        *curl;              //ʹ��curlʵ��http����
    string      httpResponseText;   //ʹ��curl����http�ķ����ı�
    BAIDU_TOKEN_INFO baidu_token_info;

public:
    TextToSound();

    /**
     * @brief ����Ҫʹ����һ��tts
     * @param ttsType
     */
    void setTypeOfTTS(TTS_TYPE ttsType);

    bool init();
    void uninit();


    /**
     * @brief �ú���ִ�о����tts����
     * @param text  Ҫת�����������ı�
     * @param param tts�Ĳ�������ͬ�����ṩ��tts���ܣ���Ҫ�Ĳ�����һ��
     * @return �����ı�ת���������ļ���ȫ�ļ�·��
     */
    string tts(string text,TTS_PARAM param);


    void tts_play(string text,TTS_PARAM param);


private:
    int tts_xunfei(const char* src_text, const char* des_path, const char* params);
    int tts_baidu(const char* src_text, const char* des_path, TTS_PARAM param);
};

#endif // TEXTTOSOUND_H
