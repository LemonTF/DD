#include "CommonTools.h"
using namespace std;

std::string CommonTools::now2string(int type/*=0*/){
	time_t now=time(0);
	if(type)
	  now=now/60*60-60;
    char _time[25] = {0};
	struct tm local_time;
	localtime_r(&now, &local_time);
	strftime(_time, 22, "%Y-%m-%d %H:%M:%S", &local_time);
	return std::string(_time);
}
uint32_t  CommonTools::str2unixtime(std::string timestr){
	tm t;
	memset(&t, 0, sizeof(tm));
	t.tm_year =  atoi(timestr.substr(0,4).c_str()) - 1900;
	t.tm_mon = atoi(timestr.substr(4,2).c_str())-1;
	t.tm_mday = atoi(timestr.substr(6,2).c_str());
	t.tm_hour = atoi(timestr.substr(8,2).c_str());
	t.tm_min = atoi(timestr.substr(10,2).c_str());
	t.tm_sec = 0;
	time_t pt = mktime(&t);

	return (uint32_t)pt;
}
std::string CommonTools::toString(int nValue){
    std::stringstream ss;
    ss << nValue;
    return ss.str();
}

std::vector<string> CommonTools::split(const std::string &str,const std::string &pattern){
	char * strc = new char[strlen(str.c_str())+1];
	strcpy(strc, str.c_str());
	std::vector<string> vec;
	char* tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL){
		vec.push_back(std::string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}
	delete[] strc;
	return vec;
}

std::string CommonTools::unixtime2str(time_t iTimeStamp){
	std::string str;
	char pszTime[32];
	tm *pTmp = localtime(&iTimeStamp);
	if (pTmp == NULL){
	  return "19000101000000";
	}

	memset(pszTime, 0, sizeof(pszTime));
	sprintf(pszTime, "%04d%02d%02d%02d%02d%02d", pTmp->tm_year + 1900, pTmp->tm_mon + 1, pTmp->tm_mday, pTmp->tm_hour, pTmp->tm_min, pTmp->tm_sec);
	//printf("************** %s *****************\n", pszTime);
	std::string s(pszTime);
	return s;
}

size_t CommonTools::receive_data(void *contents, size_t size, size_t nmemb, void *stream){
	string *str = (string*)stream;
	(*str).append((char*)contents, size*nmemb);
	return size * nmemb;
}

size_t CommonTools::writedata2file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int CommonTools::download_file(const char* url, const char outfilename[FILENAME_MAX]){
	CURL *curl;
	FILE *fp;
	CURLcode res;
	/*   调用curl_global_init()初始化libcurl  */
	res = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != res)
	{
		printf("init libcurl failed.");
		curl_global_cleanup();
		return -1;
	}
	/*  调用curl_easy_init()函数得到 easy interface型指针  */
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(outfilename,"wb");

		/*  调用curl_easy_setopt()设置传输选项 */
		res = curl_easy_setopt(curl, CURLOPT_URL, url);
		if (res != CURLE_OK)
		{
			fclose(fp);
			curl_easy_cleanup(curl);
			return -1;
		}
	    /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CommonTools::writedata2file);
		if (res != CURLE_OK){
			fclose(fp);
			curl_easy_cleanup(curl);
			return -1;
		}
		/*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		if (res != CURLE_OK)
		{
			fclose(fp);
			curl_easy_cleanup(curl);
			return -1;
		}

		res = curl_easy_perform(curl);
		// 调用curl_easy_perform()函数完成传输任务
		fclose(fp);
		/* Check for errors */
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			return -1;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		// 调用curl_easy_cleanup()释放内存

	}
	curl_global_cleanup();
	return 0;
}

CURLcode CommonTools::HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout){
    CURLcode res;
    CURL* pCURL = curl_easy_init();

    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, CommonTools::receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
    res = curl_easy_perform(pCURL);
    curl_easy_cleanup(pCURL);
    return res;
}

CURLcode CommonTools::HttpPost(const std::string & strUrl, const std::string& szJson,std::string & strResponse,int nTimeout){
    CURLcode res;
    //char szJsonData[1024];
    //memset(szJsonData, 0, sizeof(szJsonData));
    //strcpy(szJsonData, szJson.c_str());
    CURL* pCURL = curl_easy_init();
    struct curl_slist* headers = NULL;
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }
    CURLcode ret;
    ret = curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());

    ret = curl_easy_setopt(pCURL, CURLOPT_POST, 1L);
    headers = curl_slist_append(headers,"content-type:application/json");

    ret = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);

    ret = curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, szJson.c_str());
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);

    ret = curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);

    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, CommonTools::receive_data);

    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
	if(ret){}
    res = curl_easy_perform(pCURL);
    curl_easy_cleanup(pCURL);
    return res;
}

bool CommonTools::DecoderGz(uint8_t *src_data, uint32_t src_length, uint8_t *dest_data, uint32_t &dest_length)
{
    int err = 0;
    z_stream d_stream = {0};
    static char dummy_head[2] =
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in = src_data;
    d_stream.avail_in = 0;
    d_stream.next_out = dest_data;
    if(inflateInit2(&d_stream,31) != Z_OK)
        return false;
    while (d_stream.total_out < dest_length && d_stream.total_in < src_length) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK ) {
            if(err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)  {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    if(inflateEnd(&d_stream) != Z_OK)
        return false;
    dest_length = (uint32_t)d_stream.total_out;
    return true;
}

string CommonTools::GetPoiDesc(string &pointstr){
	if(pointstr.empty()) return "";
	std::vector<string> ss = CommonTools::split(pointstr,",");
	if(ss.size()<2) return "";
	//string szURL = "http://100.69.238.11:8000/map/mapapi/reversegeo?product_id=364&acc_key=RGJTN-1JD6N-V6N0N-RFV1T-QRAKR-4RLPK&requester_type=inner.traffic-info-platform&coordinate_type=gcj02&map_type=tmap&app_version=100.100.100&app_id=didi_service&platform=inner.traffic-info-platform&select_lng="+ss[0]+"&select_lat="+ss[1]+"&lang=zh-CN";
	string szURL = "http://10.85.129.110:8020/api/mapapi/reversegeo?token=3ee5fbd5fec111ecd0923268744aade9&product_id=364&acc_key=RGJTN-1JD6N-V6N0N-RFV1T-QRAKR-4RLPK&requester_type=inner.traffic-info-platform&coordinate_type=gcj02&map_type=tmap&app_version=100.100.100&app_id=didi_service&platform=inner.traffic-info-platform&select_lng="+ss[0]+"&select_lat="+ss[1]+"&lang=zh-CN";
	//std::cout<<szURL<<std::endl;
	string strResponse = "";
	CURLcode nRes = CommonTools::HttpGet(szURL, strResponse,300);
	if (nRes != CURLE_OK || strResponse == ""){
		char strMsg[512] = {0};
		sprintf(strMsg,"error,code:%d",nRes);
		//strMsg += szURL;
		std::cout<<"error:"<<strMsg<<",response:"<<strResponse<<std::endl;
		return "";
	}
//	else{
//		std::cout<<"success:"<<",response:"<<strResponse<<std::endl;
//	}
	return CommonTools::decodePoiNamefromJson(strResponse);
}
std::string CommonTools::getMapVersion(){
	//std::string postURL="http://100.69.81.14:8084/congest/map_version";
	std::string postURL="http://10.85.129.110:8020/api/congest/map_version?token=3ee5fbd5fec111ecd0923268744aade9";
	string strResponse = "";
	CURLcode nRes2 = CommonTools::HttpPost(postURL,"",strResponse,300);
	if (nRes2 != CURLE_OK || strResponse == ""){
		char strMsg2[512] = {0};
		sprintf(strMsg2,"error,code:%d",nRes2);
		//strMsg += szURL;
		std::cout<<"error:"<<strMsg2<<",response:"<<strResponse<<std::endl;
		return "";
	}
	Json::Reader  reader;
	Json::Value   value;
	reader.parse(strResponse,value);
	return value["res"]["version_str"].asString();
}
int CommonTools::submitTTIJob(const std::string &links,const std::string &roadname,int province_id,int cityid,const std::string &mapversion){
	std::string postURL = "http://100.69.96.20:8001/tti/add_tti_obj_by_linkid?token=c7ffd16cb14e8cf17517c49b20e7f561";
	std::string strResponse = "";
	//char strJson[512] = {0};
	time_t now=time(0);
	time_t btime=now-89*24*60*60;
	std::string etime=CommonTools::unixtime2str(now);
	etime.pop_back();etime.pop_back();
	std::string stime=CommonTools::unixtime2str(btime);
	stime.pop_back();stime.pop_back();

	std::string cityname;
	if(cityid==4403)
		cityname="深圳市";
	std::stringstream ss;
	ss<<"{\"links\":\""<<links<<"\",\"esiwei\":1,\"version\":\""<<mapversion<<"\",\"stime\":\""<<stime<<"\",\"etime\":\""<<etime<<"\",\"city\":\""<<cityname<<"\",\"provinces\":\""<<province_id<<"\",\"name\":\""<<roadname<<"\",\"desc\":\"拥堵点的tti回溯\",\"calc_type\":1,\"tti_dir\":0}";
	//sprintf(strJson,"{\"links\":\"%s\", \"esiwei\":1,\"version\":\"%s\",\"stime\":\"%s\",\"etime\":\"%s\", \"city\": \"%s\", \"provinces\": \"%d\",\"name\":\"%s\",\"desc\":\"拥堵点的tti回溯\",\"calc_type\":1,\"tti_dir\":0}",links.c_str(),mapversion.c_str(),stime.c_str(),etime.c_str(),cityname.c_str(),province_id,roadname.c_str());
	CURLcode nRes2 = CommonTools::HttpPost(postURL,ss.str(),strResponse,300);
	if (nRes2 != CURLE_OK || strResponse == ""){
		char strMsg2[512] = {0};
		sprintf(strMsg2,"error,code:%d",nRes2);
		//strMsg += szURL;
		std::cout<<"error:"<<strMsg2<<",response:"<<strResponse<<std::endl;
		return 0;
	}
	Json::Reader  reader;
	Json::Value   value;
	reader.parse(strResponse,value);
	return value["res"]["job_id"].asInt();
}
string CommonTools::GetRoadNameByLinkID(std::uint64_t linkid,std::string map_version){
	string postURL = "http://100.69.187.40:8080/link_query/linkid_with_node";
	string strResponse = "";
	char strJson[1024] = {0};
	sprintf(strJson,"{\"id\":\"%lu\",\"version\":\"%s\",\"origin\":\"0\"}",linkid,map_version.c_str());

	//std::cout<<"json str="<<strJson<<endl;
	CURLcode nRes2 = CommonTools::HttpPost(postURL,strJson,strResponse,300);

	if (nRes2 != CURLE_OK || strResponse == ""){
		char strMsg2[512] = {0};
		sprintf(strMsg2,"error,code:%d",nRes2);
		//strMsg += szURL;
		std::cout<<"error:"<<strMsg2<<",response:"<<strResponse<<std::endl;
		return "";
	}

	//std::cout<<"strResponse:"<<strResponse<<std::endl;
	//std::cout<<"json str="<<strResponse<<endl;
	return CommonTools::decodeRoadNamefromJson(strResponse);
}
void CommonTools::getAdmincode(uint64_t linkid,std::string&name,std::string&admincode){
	string postURL = "http://100.69.96.20:8005/linkifo/get_roadnameAndadmincode_by_roadid";
	string strResponse = "";
	char strJson[512] = {0};
	sprintf(strJson,"{\"roadid\":\"%lu\"}",linkid);

	//std::cout<<"json str="<<strJson<<endl;
	CURLcode nRes2 = CommonTools::HttpPost(postURL,strJson,strResponse,300);

	if (nRes2 != CURLE_OK || strResponse == ""){
		char strMsg2[512] = {0};
		sprintf(strMsg2,"error,code:%d",nRes2);
		//strMsg += szURL;
		std::cout<<"error:"<<strMsg2<<",response:"<<strResponse<<std::endl;
	}

	Json::Reader  reader;
	Json::Value   value;
	reader.parse(strResponse,value);
	name=value["res"]["name"].asString();
	admincode=value["res"]["admin_code"].asString();
	//std::cout <<"lemon:"<<linkid<<":"<<name<<":"<<admincode<<std::endl;
}
string CommonTools::decodePoiNamefromJson(string jsonstr){
	Json::Reader  reader;
	Json::Value   value;
	reader.parse(jsonstr,value);
	int _size = value["rgeo_result"].size();
	string roadname = "";
	for(int i = 0; i < _size; ++i) {
		// 遍历数组
		Json::Value val_image = value["rgeo_result"][i];
		if(!val_image["base_info"].isNull()){
			roadname = val_image["base_info"]["displayname"].asString();
			break;
		}
	}
	return roadname;
}

string CommonTools::decodeRoadNamefromJson(string jsonstr){
	Json::Reader  reader;
	Json::Value   value;
	reader.parse(jsonstr,value);
	const Json::Value fs = value["features"];
	int file_size = value["features"].size();
	string roadname = "";
	for(int i = 0; i < file_size; ++i) {
		// 遍历数组
		Json::Value val_image = value["features"][i]["properties"];
		if (!val_image["pathname"].isNull()&&!val_image["admincoder"].isNull()){
			roadname = val_image["pathname"].asString()+ ";" +val_image["admincoder"].asString();
			break;
		}
	}
	return roadname;
}


