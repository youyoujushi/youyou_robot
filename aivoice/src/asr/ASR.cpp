#include "ASR.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"




ASR::ASR()
{
}

ASR::~ASR()
{
}


int ASR::init_xunfei(speech_rec_notifier asr_notifer){

    if(isInited)
        return 0;

    this->asrType = ASR_TYPE_XUNFEI;
    this->asr_notifer = asr_notifer;

    int ret = MSP_SUCCESS;
	/* login params, please do keep the appid correct */
	const char* login_params = "appid = 59450ad8, work_dir = .";
    //  const char* login_params = "appid = 56ee43d0, work_dir = .";//树莓派

	/*
	* See "iFlytek MSC Reference Manual"
	*/
    const char* session_begin_params =
        "sub = iat, domain = iat, language = zh_cn, "
        "accent = mandarin, sample_rate = 16000, "
        "result_type = plain, result_encoding = utf8";
    int errcode = sr_init(&iat, session_begin_params, SR_MIC, &this->asr_notifer);
    if (errcode) {
        printf("speech recognizer init failed\n");
        return -1;
    }

	ret = MSPLogin(NULL, NULL, login_params);
	if (MSP_SUCCESS != ret)	{
		printf("MSPLogin failed , Error code %d.\n",ret);
		return -1;
	}


    isInited = true;
    return 1;
}


int ASR::uninit(){
    if(!isInited)
      return 0;

    MSPLogout();
    sr_uninit(&iat);
    isAsring = false;
    isInited = false;
    return 1;
}

int ASR::startAsr(){

    if(isAsring)
        return 0;
    if(!isInited)
      return 0;

    int errcode = sr_start_listening(&iat);         //开始录音并识别
	if (errcode) {
		printf("start listen failed %d\n", errcode);
//        sr_uninit(&iat);
        return -1;
	}

    isAsring = true;
    return 1;
}

int ASR::stopAsr(){

    if(!isAsring)
        return 0;
    if(!isInited)
      return 0;

    int errcode = sr_stop_listening(&iat);         //停止录音和识别
	if (errcode) {
		printf("stop listen failed %d\n", errcode);
//        sr_uninit(&iat);
        return -1;
	}

    isAsring = false;
    return 1;
}
