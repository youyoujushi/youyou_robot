#include "ros/ros.h"
#include "std_msgs/String.h"
#include "nlp_msg.h"
#include "tts_msg.h"
#include "CNlp.h"
#include "tts/texttosound.h"
#include <sstream>

using namespace std;
using namespace ros;

ros::Publisher tts_pub; 
CNlp nlp;

void asrResultCallback(const aivoice::nlp_msgConstPtr& msg){//(const std_msgs::String::ConstPtr& msg){

  char buf[512];
  sprintf(buf,"recv nlp_msg:\n \
           text:%s",((string)msg->text).data());
  ROS_INFO("%s",buf);

  string text = (string)msg->text;
  string voice = (string)msg->voice;
  if(text.length() <= 0)
  	return;

  string result = nlp.process(text);

  if(result.length() > 0){
	aivoice::tts_msg msg;
	msg.tts_type = TTS_XUNFEI;
  	msg.voice_name = voice.length() == 0 ? "jinger" : voice;
  	msg.text = result;
  	msg.text_encoding = "utf8";
    tts_pub.publish(msg); 
	ROS_DEBUG("%s","发送tts");
  }

}

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char *argv[])
{

// string result = nlp.process("你好啊");

//   if(result.length() > 0){
// 	aivoice::tts_msg msg;
//   	msg.voice_name = "xiaomei";
//   	msg.text = result;
//   	msg.text_encoding = "utf8";
//     tts_pub.publish(msg); 
// 	ROS_DEBUG("%s","发送tts");
//   }
//   return 0;
	ros::init(argc, argv, "nlp_node");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe<aivoice::nlp_msg>("nlp_topic", 1,asrResultCallback);
	tts_pub = n.advertise<aivoice::tts_msg>("tts_topic", 10);
	ros::spin();
	return 0;
}