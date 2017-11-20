#include "global.h"

#include "FaceRecActionClient.h"



#define	BUFFER_SIZE	4096

FaceRecActionClient *faceRecClient = NULL;

Publisher asr_pub;
Publisher tts_pub;
Publisher tran_pub;
Publisher nlp_pub;
Subscriber asr_ctrl_sub;
Subscriber speak_notify_sub;
ASR asr;
bool robot_speaking = false;

PocketSphinxAsr psAsr("/usr/local/share/pocketsphinx/model/zh/hmm",
                        "/usr/local/share/pocketsphinx/model/zh/1271.lm",
                        "/usr/local/share/pocketsphinx/model/zh/1271.dic");
// PocketSphinxAsr psAsr("/usr/local/share/pocketsphinx/model/en-us/en-us",
//                         "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin",
//                         "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict");
// PocketSphinxAsr psAsr("/usr/local/src/pocketsphinx-5prealpha/model/cn/zh_broadcastnews_ptm256_8000",
//                         "/usr/local/src/pocketsphinx-5prealpha/model/cn/TAR1271/1271.lm",
//                         "/usr/local/src/pocketsphinx-5prealpha/model/cn/TAR1271/1271.dic");

string voiceName = "xiaokun";   //当前的发言人
string lastVoiceName = voiceName;
bool    ignored  = false;    //是否忽略识别出的结果
bool    isWakeUp = false;   //是否已唤醒，未唤醒状态使用离线语音识别，唤醒状态使用在线识别
bool    waitToAsr   = false;//是否正等待消息以开始asr，发布asr结果消息时置真，收到开始放音消息时，置假
time_t  beginDetectSilenceTime = 0; //开始检测静默状态的时间 

// Timer   silence_check_timer;

/****************讯飞asr回调函数*****************/
static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

bool handleAsrResult(char *text);
bool containWords(char* str1,const char** wordArray,int len);
bool containWordsInOrder(char* str1,const char** wordArray,int len);

void* closeFlyAsrThread(void* arg);//关闭讯飞识别线程的线程，因为识别结果回调函数是在asr线程内部调用的，所以要另外开一个线程关闭asr线程

struct TRANSLATE_INFO{
    bool isTranslate;
    string srcLanguage;
    string destLanguage;
}tran_info;

enum ASR_STATE{
    asr_off,        //关闭状态，需要外部命令开启语音识别
    asr_silence,    //静默状态，离线语音识别等待命令词唤醒
    asr_wakeup,     //已唤醒状态，切换到在线识别
    asr_chat,       //在线对话模式
}asr_state;



//讯线语音识别结果回调函数 
void on_result(const char *result, char is_last)
{
	if (result) {
		size_t left = g_buffersize - 1 - strlen(g_result);
		size_t size = strlen(result);
		if (left < size) {
			g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
			if (g_result)
				g_buffersize += BUFFER_SIZE;
			else {
				printf("mem alloc failed\n");
				return;
			}
		}
		strncat(g_result, result, size);

        if(is_last){

            // ignored = false;
            if(robot_speaking)
                return;

            //发布识别结果消息
            pub_asr(g_result,voiceName);

            //先节点内过滤识别结果
            if(handleAsrResult(g_result)){
                beginDetectSilenceTime = time(NULL);
                return;
            }

            if(tran_info.isTranslate){//当前为翻译状态
                beginDetectSilenceTime = time(NULL);
                pub_trans(g_result,tran_info.srcLanguage,tran_info.destLanguage,"baidu");
                return;
            }

            // 发布nlp消息
            pub_nlp(g_result,voiceName);
            beginDetectSilenceTime = time(NULL);

            // if(asr_state == asr_chat){
            //     beginDetectSilenceTime = time(NULL);
            //     // 发布nlp消息
            //     pub_nlp(g_result,voiceName);
            // }else{
            //     ROS_DEBUG("%s","cotinue asr...");
            //     // asr.startAsr();
            //     ignored = true;
            // }
        }
	}
}

void on_speech_begin()
{
	if (g_result)
	{
		free(g_result);
	}
	g_result = (char*)malloc(BUFFER_SIZE);
	g_buffersize = BUFFER_SIZE;
	memset(g_result, 0, g_buffersize);

	printf("Start Listening...\n");
    // system("play start_record.wav");
    // sleep(1);
}

void on_speech_end(int reason)
{
	if (reason == END_REASON_VAD_DETECT){
		printf("\nSpeaking done \n");
		//结束上一段asr，开始新的asr
        asr.stopAsr();
        //  system("play stop_record.wav");
        //  sleep(1);
        if((strlen(g_result) <= 0 || ignored) && robot_speaking == false)
             asr.startAsr();
        else
            waitToAsr = true;
	}else{
		printf("\nRecognizer error %d\n", reason);
    }
}

//pocketsphinx 语音识别结果回调函数 
void psAsrCallback(string result){
    ROS_DEBUG("asr识别结果：%s",result.c_str());
    cout<<"asr识别结果："<<result.c_str()<<endl;

    pub_asr(result,voiceName);

    if(result.compare("优优") == 0 && !isWakeUp){//检测到唤酷词，从静默状态转到在线识别状态
        psAsr.stopListen();//关闭离线识别
        asr_state = asr_wakeup;
        beginDetectSilenceTime = time(NULL);//开始在线识别时就启动静默检测

        speech_rec_notifier notifier = {on_result, on_speech_begin, on_speech_end };
        int result = 0;
        if( (result =asr.init_xunfei(notifier)) != 1){  //开启在线识别
             ROS_WARN("init_xunfei return %d",result);
        }else{
            if( (result =asr.startAsr()) != 1){
                ROS_WARN("startAsr return %d",result);
            }else
                ROS_INFO("%s","Start ASR success!");
        }

        char* msgs[] = {"在的，什么事","你好","在，有什么吩咐","是在叫我吗"};
        srand(time(0));
        pub_tts(msgs[rand()%4],voiceName);
    }
}


/*****************************/
void speakNotifyCallback(const aivoice::speak_notify_msgConstPtr& msg){
    char buf[512];
    sprintf(buf,"recv speak_notify_msg:\n \
           speak_state:%d", msg->speak_state);
     ROS_DEBUG("%s",buf);


    int speak_state = msg->speak_state;
    if(speak_state == 0 ){//放音开始通知
        ROS_DEBUG("recv speaking start");
        robot_speaking = true;
        if(asr.isAsring ){
            asr.stopAsr();
        }
    }else if(speak_state == 1 ){//放音结束通知
        ROS_DEBUG("recv speaking stop");
        robot_speaking = false;
        waitToAsr = false;
        if(!asr.isAsring){
            asr.startAsr();
        }
    }
}

void asrCtrlCallback(const aivoice::asr_ctrl_msgConstPtr& msg){//(const std_msgs::String::ConstPtr& msg){

  char buf[512];
  sprintf(buf,"recv asr contrl msg:\n \
           order:%d \
					 asrType:%d", msg->order,msg->asrType);
  ROS_DEBUG("%s",buf);

  int order     = msg->order;
  int asrType   = msg->asrType;

  if(order == 1){//停止asr

    asr_state = asr_off;

    if(!isWakeUp){//停止离线识别
        psAsr.stopListen();
        return;
    }

    if(asr.uninit() == 1){
        ROS_INFO("%s","Stop ASR success!");
    }else{
        ROS_WARN("%s","Stop ASR Failed!");
    }

  }else if(order == 0){//开始asr

    if(!isWakeUp){//开始离线识别
        psAsr.beginListen(psAsrCallback);
        asr_state = asr_silence;
        return;
    }

    asr_state = asr_wakeup;
    if(asrType == ASR_TYPE_XUNFEI){

        speech_rec_notifier notifier = {on_result, on_speech_begin, on_speech_end };
        int result = 0;
        if( (result =asr.init_xunfei(notifier)) != 1){
             ROS_WARN("init_xunfei return %d",result);
        }else{
            if( (result =asr.startAsr()) != 1){
                ROS_WARN("startAsr return %d",result);
            }else
                ROS_INFO("%s","Start ASR success!");
        }
    }

  }


}



//预处理语音识别结果，用最简单的方法直接比较文本，如果有特定的命令词组出现就做相应的处理，不再发送图灵机器人
bool handleAsrResult(char *text){

    const char* pattern0[] = {"停止翻译"};
    const char* pattern1[] = {"讲","普通话","男声"};
    const char* pattern2[] = {"讲","普通话","女声"};
    const char* pattern3[] = {"讲","普通话"};
    const char* pattern4[] = {"讲","广东话"};
    const char* pattern5[] = {"讲","四川话"};
    const char* pattern6[] = {"讲","湖南话"};
    const char* pattern7[] = {"讲","台湾话"};
    const char* pattern8[] = {"讲","河南话"};
    const char* pattern9[] = {"讲","东北话"};
    const char* pattern10[] = {"讲","童声"};
    const char* pattern11[] = {"讲","英文"};
    const char* pattern12[] = {"讲","英语"};
    const char* pattern13[] = {"讲","粤语"};
    const char* pattern14[] = {"中文","翻译","英文"};
    const char* pattern15[] = {"我说","中文","你说","英文"};
    const char* pattern16[] = {"英文","翻译","中文"};
    const char* pattern17[] = {"我说","英文","你说","中文"};
    const char* pattern18[] = {"不","聊"};
    const char* pattern19[] = {"待命"};
    const char* pattern20[] = {"聊下天"};
    const char* pattern21[] = {"说说话"};
    const char* pattern22[] = {"我是谁"};

    char *reply = "好的";
    if(containWords(text,pattern1,3)){//普通话男声
        voiceName = "yufeng";
    }else if(containWords(text,pattern2,3)){//普通话女声
        voiceName = "jinger";
    }else if(containWords(text,pattern3,2)){//普通话
        voiceName = "jinger";
    }else if(containWords(text,pattern4,2)){//广东话
        voiceName = "xiaomei";
    }else if(containWords(text,pattern5,2)){//四川话
        voiceName = "xiaorong";
    }else if(containWords(text,pattern6,2)){//湖南话
        voiceName = "xiaoqiang";
    }else if(containWords(text,pattern7,2)){//台湾话
        voiceName = "xiaolin";
    }else if(containWords(text,pattern8,2)){//河南话
        voiceName = "xiaokun";
    }else if(containWords(text,pattern9,2)){//东北话
        voiceName = "xiaoqian";
    }else if(containWords(text,pattern10,2)){//童声
        voiceName = "nannan";
    }else if(containWords(text,pattern11,2)){//英文
        voiceName = "catherine";
        reply = "ok";
    }else if(containWords(text,pattern12,2)){//英语
        voiceName = "catherine";
        reply = "ok";
    }else if(containWords(text,pattern13,2)){//粤语
        voiceName = "xiaomei";
    }else if(containWordsInOrder(text,pattern14,3)){//中译英
        lastVoiceName = voiceName;
        voiceName = "catherine";
        tran_info.isTranslate = true;
        tran_info.srcLanguage = "zh";
        tran_info.destLanguage = "en";
        reply = "ok";
    }else if(containWordsInOrder(text,pattern15,4)){//中译英
        lastVoiceName = voiceName;
        voiceName = "catherine";
        tran_info.isTranslate = true;
        tran_info.srcLanguage = "zh";
        tran_info.destLanguage = "en";
        reply = "ok";
    }else if(containWordsInOrder(text,pattern16,3)){//英译中
        lastVoiceName = voiceName;
        voiceName = "jinger";
        tran_info.isTranslate = true;
        tran_info.srcLanguage = "en";
        tran_info.destLanguage = "zh";
    }else if(containWordsInOrder(text,pattern17,4)){//英译中
        lastVoiceName = voiceName;
        voiceName = "jinger";
        tran_info.isTranslate = true;
        tran_info.srcLanguage = "en";
        tran_info.destLanguage = "zh";
    }else if(containWords(text,pattern0,1)){//停止翻译
        // voiceName = "jinger";
        voiceName = lastVoiceName;
        memset(&tran_info,0,sizeof(tran_info));
    }else if(containWords(text,pattern18,2)){//结束聊天
        asr_state = asr_wakeup;
        char* msgs[] = {"好的，再见了，我会想你的","下次再聊","好的，快点回来哟","拜拜"};
        srand(time(0));
        reply = msgs[rand()%4];
        ROS_INFO("%s","结束聊天");
    }else if(containWords(text,pattern19,1)){//待机，切换离线
        pthread_t tid;
        pthread_create(&tid,NULL,closeFlyAsrThread,NULL);
        char* msgs[] = {"好的，已进入静默状态","好的，需要时请叫我的名字：优优","已待机"};
        srand(time(0));
        reply = msgs[rand()%3];
        ROS_INFO("%s","切换离线识别，进入待机状态");
    }else if(containWords(text,pattern20,1) || containWords(text,pattern21,1)){//开始聊天
        asr_state = asr_chat;
        char* msgs[] = {"好的，你想聊些什么","好的，我正想找人说说话呢","有什么悄悄话要对我说吗"};
        srand(time(0));
        reply = msgs[rand()%3];
        ROS_INFO("%s","开始聊天");
    }else if(containWords(text,pattern22,1)){//我是谁
        // voiceName = "jinger";
        reply = "让我来仔细瞧一瞧";

        face_recGoal goal; 
        goal.timeout = 6;
        if(faceRecClient != NULL)
            faceRecClient->sendGoal(goal);
    }else{
        return false;
    }

    pub_tts(reply,voiceName);
    return true;

}

//检测数组中所有字符串是否在str1中出现
bool containWords(char* str1,const char** wordArray,int len){

    if(wordArray == NULL || len == 0)
        return false;

    string content(str1); 
    for(int i=0;i<len;i++){
        if(content.find(wordArray[i]) == string::npos)
            return false;
    }
    return true;
    
}

//检查数组中的词是否按顺序出现
bool containWordsInOrder(char* str1,const char** wordArray,int len){

    if(wordArray == NULL || len == 0)
        return false;

    string content(str1); 
    int lastPost = -1;
    for(int i=0;i<len;i++){
        int pos = content.find(wordArray[i]);
        if(pos == string::npos || pos <= lastPost)
            return false;
        lastPost = pos;
    }
    return true;
    
}

void* closeFlyAsrThread(void* arg){
    asr_state = asr_silence;
    asr.uninit();  //关闭在线识别
    psAsr.beginListen(psAsrCallback);   //开启离线识别
    beginDetectSilenceTime = 0;
    return NULL;
}

//在等待tts放音时，asr线程是关闭的，如果tts放音失败，asr就再也不能启动。该线程确保tts消息超时后，asr自动激活
void* guaranteeAsrWork(void* arg){
    time_t beginTime = 0;
    int timeOut = 30;
    while(1){
        if(waitToAsr){
            if(beginTime == 0)
                beginTime = time(NULL);
            if(time(NULL)-beginTime > timeOut){
                robot_speaking = false;
                asr.startAsr();
                waitToAsr = false;
                beginTime = 0;
                ROS_WARN("%s","recv speakinng msg timeout ,restart asr");
            }
        }else{
            beginTime = 0;
        }

        if( beginDetectSilenceTime > 0 
            && time(NULL)-beginDetectSilenceTime > silence_timeout
            && asr_state != asr_silence){
                pthread_t tid;
            pthread_create(&tid,NULL,closeFlyAsrThread,NULL);
            pub_tts("静默",voiceName);
        }

        usleep(10000);
    }
    return NULL;
}

void pub_tts(string text,string voice){
    aivoice::tts_msg msg;
	msg.tts_type = 1;
  	msg.voice_name = voice;
    msg.text = text;
  	msg.text_encoding = "utf8";
    tts_pub.publish(msg); 
	ROS_DEBUG("发送tts %s",text);
}

void pub_asr(string text,string voice){
    ROS_DEBUG("asr result：%s",text);
    aivoice::asr_result_msg msg;
    msg.text = text;
    msg.voice = voice;
    asr_pub.publish(msg);
}

void pub_nlp(string text,string voice){
    ROS_DEBUG("nlp request：%s",text);
    aivoice::nlp_msg msg;
    msg.text = text;
    msg.voice = voice;
    nlp_pub.publish(msg);
}

void pub_trans(string text,string srcLang,string destLang,string type){
    ROS_DEBUG("发送translate_request_msg:%s",text);
    aivoice::translate_request_msg msg;
    msg.srcText = text;
    msg.srcLanguage = srcLang;
    msg.destLanguage = destLang;
    msg.type = type;
    tran_pub.publish(msg);
}

int main(int argc,char** argv){

    //启动后，开启离线识别，等待命令词唤醒
    if(psAsr.beginListen(psAsrCallback))
        asr_state = asr_silence;
    else{
        asr_state = asr_off; 
        ROS_ERROR("%s","离线识别启动失败");
    }

    //开启asr重启线程
    pthread_t tid;
    pthread_create(&tid,NULL,guaranteeAsrWork,NULL);

    init(argc,argv,"asr_node");
    NodeHandle n;
    // NodeHandle n2;
    // NodeHandle n3;
    // NodeHandle n4;
    // NodeHandle n5;
    // NodeHandle n6;    
    asr_ctrl_sub = n.subscribe("asr_ctrl_topic",10,asrCtrlCallback); 
    speak_notify_sub = n.subscribe("speak_notify_topic",10,speakNotifyCallback);
    asr_pub = n.advertise<aivoice::asr_result_msg>("asr_result_topic",10);
    nlp_pub = n.advertise<aivoice::nlp_msg>("nlp_topic",10);
    tts_pub = n.advertise<aivoice::tts_msg>("tts_topic",10);
    tran_pub = n.advertise<aivoice::translate_request_msg>("translate_topic",10);

    faceRecClient = new FaceRecActionClient();

    // silence_check_timer = n.createTimer(ros::Duration(0.1), timerCallback);
    spin();

    return 0;
}