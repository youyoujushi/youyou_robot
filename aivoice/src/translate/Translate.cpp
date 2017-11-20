
#include "Translate.h"
#include "md5.h"
#include "json.hpp"

// #include <openssl/md5.h>

using json = nlohmann::json;

Translate::Translate()
{
}

Translate::~Translate()
{
}



string Translate::tran(string content,string srcLang,string destLang){

   string salt = "1804289383";
   string appid = "20170718000064967";
   string sign = appid;
   sign += content + salt + "IYltr985qGLp9lyMDthX";


   string url = "http://api.fanyi.baidu.com/api/trans/vip/translate";
   url +=   "?q="+content + 
            "&from=" + srcLang +
            "&to=" + destLang +
            "&appid=" + appid +
            "&salt=" + salt +
            "&sign=" + MD5(sign).toStr();

    auto fr = cpr::GetAsync(cpr::Url{url},cpr::Timeout{5000});
    fr.wait(); // This waits until the request is complete
    auto r = fr.get(); // Since the request is complete, this returns immediately

    if(r.text.length() > 0){

        std::cout << r.text << std::endl;

        json js = json::parse(r.text);
        if(js.is_object()){
            json result = js["trans_result"];
            if(result.is_null()){
                return "";
            }else{
                if(result.is_array() && result.size() > 0){
                    string dest = result[0]["dst"];
                    if(dest.length() > 0)
                        return dest;
                }
            }
        }
        // baidu_token_info.expires = time(NULL)+ js["expires_in"].get<time_t>();
        // baidu_token_info.token  = js["access_token"];
    }

    

    return "";
}