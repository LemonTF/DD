#include <iostream>
#include <curl/curl.h>
#include "zlib.h"
#include <vector>
#include <string>
#include <unistd.h>
#include <memory.h>
#include <json/json.h>
#include <sstream>
using namespace std;
class CommonTools{
	public:
		CommonTools();
		~CommonTools();
	public:
		static std::string now2string(int type=0);
		static std::string toString(int nValue);
		// 将字符串转化为 unixtime 字符串的格式为 2018-07-22 01:00:02
		static uint32_t str2unixtime(std::string timestr);
		// 将unixtime 转化为字符串 20180201001100
		static std::string unixtime2str(time_t iTimeStamp);
		// Get 和 Post 请求的回掉数据接收函数
		static size_t receive_data(void *contents, size_t size, size_t nmemb, void *stream);
		// HTTP 下载文件的回掉函数
		static size_t writedata2file(void *ptr, size_t size, size_t nmemb, FILE *stream);
		// 文件下载接口
		static int download_file(const char* url, const char outfilename[FILENAME_MAX]);
		// http get 请求
		static CURLcode HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout);
		// htpp post 请求
		static CURLcode HttpPost(const std::string & strUrl,const std::string &szJson,std::string & strResponse,int nTimeout);

		static bool DecoderGz(uint8_t *src_data, uint32_t src_length, uint8_t *dest_data, uint32_t &dest_length);

		static std::vector<string> split(const std::string &str,const std::string &pattern);

		static std::string decodeRoadNamefromJson(string jsonstr);

		static std::string decodePoiNamefromJson(string jsonstr);

		static std::string GetPoiDesc(string &pointstr);
		static void getAdmincode(uint64_t linkid,std::string&,std::string&);

		static std::string GetRoadNameByLinkID(std::uint64_t linkid,std::string map_version);
		static int submitTTIJob(const std::string &links,const std::string &roadname,int province_id,int cityid,const std::string &mapversion);
		static std::string getMapVersion();
};

