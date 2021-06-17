#include <iostream>
#include "LogUtils.h"
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include "mydb.h"
#include "config_file.h"
#include "overallDataManagement.h"
#include "CommonTools.h"
std::string exec(const char *cmd){
	NOTICE("exec_cmd_begin:"<<cmd);
	std::string maxLine;
	auto pipe=popen(cmd,"r");
	if(!pipe)return maxLine;
	char buffer[128]{0};
	while(!feof(pipe)){
		if(fgets(buffer,128,pipe)!=NULL){
			std::string line(buffer);
			auto pos =line.find_first_of('/');
			if(pos!=std::string::npos){
				auto subline=line.substr(pos,line.size()-1-pos);
				if(maxLine.empty()){maxLine=subline;
				}else{
					if(maxLine<subline)maxLine=subline;
				}
			}
		}
	}
	NOTICE("exec_cmd_end:"<<cmd);
	return maxLine;
}
bool loadDataFromHDFS(){
	bool ret=false;
	static std::string lastVersion;
	std::string version=exec("HADOOP_USER_PASSWORD=5ePbojWg1vSk94GZgpIiH0CkJSiAyQuZ hadoop dfs -ls /user/xinsi_traffic/f3mapdata/didimap/");
	if(!version.empty()){
		if(lastVersion.empty()){
			lastVersion=version;
			return ret;
		}
		if(lastVersion != version){
			lastVersion=version;
			std::string cmd="HADOOP_USER_PASSWORD=5ePbojWg1vSk94GZgpIiH0CkJSiAyQuZ hadoop dfs -get "+version +" ../etc/mapdata/didimap/";
			exec(cmd.c_str());
			ret=true;
		}
	}
	return ret;
}
#if 0
bool getAndInitMapBigData(){
	static std::string v;
	std::string version=CommonTools::getMapVersion();
	NOTICE(version<<"===" <<v);
	bool b=false;
	if(!version.empty() && v != version){
		std::string tmpv=v;
		v=version;
		b=true;
		std::string path="../etc/"+v+"/";
		DIR * d=opendir(path.c_str());
		if(d==NULL){
			std::string cmd="mkdir -p " +path;
			system(cmd.c_str());
		}else{
			closedir(d);
		}
		
		std::string topo_path=path+"topoinfo_partition_0.dat";
		std::string road_path=path+"roadinfo_partition_0.dat";
		std::string coor_path=path+"coor_partition_0.dat";

		std::string topo_path_copy=topo_path+"._COPYING_";
		std::string road_path_copy=road_path+"._COPYING_";
		std::string coor_path_copy=coor_path+"._COPYING_";
		//if((0==access(topo_path_copy.c_str(),F_OK)) || (0==access(road_path_copy.c_str(),F_OK)) || (0==access(coor_path_copy.c_str(),F_OK))){
		unlink(topo_path_copy.c_str());
		unlink(road_path_copy.c_str());
		unlink(coor_path_copy.c_str());

		unlink(topo_path.c_str());
		unlink(road_path.c_str());
		unlink(coor_path.c_str());
	//	}else if((-1==access(topo_path.c_str(),F_OK)) || (-1==access(road_path.c_str(),F_OK)) || (-1==access(coor_path.c_str(),F_OK))){
	//		unlink(topo_path.c_str());
	//		unlink(road_path.c_str());
	//		unlink(coor_path.c_str());
	//	}
		std::string hadoop_path="HADOOP_USER_PASSWORD=5ePbojWg1vSk94GZgpIiH0CkJSiAyQuZ hadoop fs -get hdfs://DClusterNmg3/hdp/map/masami/NewPipeLine/release_data/"+v+"/rp3rd/data.new/"+v;
		std::string hadoop_coor=hadoop_path+"/coor_partition_0.dat "+path;
		system(hadoop_coor.c_str());
		NOTICE("hadoop"+hadoop_coor+" Over");
		std::string hadoop_road=hadoop_path+"/roadinfo_partition_0.dat "+path;
		system(hadoop_road.c_str());
		NOTICE("hadoop"+hadoop_road+" Over");
		std::string hadoop_topo=hadoop_path+"/topoinfo_partition_0.dat "+path;
		system(hadoop_topo.c_str());
		NOTICE("hadoop"+hadoop_topo+" Over");
		std::cout <<"HDFS File Get OK!"<<std::endl;

		NOTICE("Start Init MapData To Memory....");
		if(!mapManagerInstance::inst()->init(path)){
			NOTICE("MapData Init Failed..");
			b=false;
		}else{
			if(!tmpv.empty()){
				std::string path="../etc/"+tmpv+"/";
				std::string topo_path=path+"topoinfo_partition_0.dat";
				std::string road_path=path+"roadinfo_partition_0.dat";
				std::string coor_path=path+"coor_partition_0.dat";

				unlink(topo_path.c_str());
				unlink(road_path.c_str());
				unlink(coor_path.c_str());
			}
		}
	}
	return b;
}
#endif
int main(){	
	std::string mapPath;
	std::string citylist;
	std::string extra_info;
    bool flag=false;
    do{
        config_file cf;
        cf.open("./config.ini");
        std::string log_name=cf.get("service","log_name","");
        std::string dbkey=cf.get("service","db_logon","");
        mapPath=cf.get("service","map_path","");
		citylist=cf.get("service","province_code","");
		extra_info=cf.get("service","tti_job","");
		std::string host=cf.get("mysql","host","");
		std::string port=cf.get("mysql","port","");
		std::string user=cf.get("mysql","user","");
		std::string password=cf.get("mysql","password","");
		std::string database=cf.get("mysql","database","");

        if(log_name.empty() || dbkey.empty() || mapPath.empty()){
            flag=true;
            break;
        }
    	LogUtils::instance().setLogName(log_name);
    	if(!LogUtils::instance().open_log()){
            NOTICE("Open Log Error");
            flag=true;
            break;
        }
		NOTICE("citylist="<<citylist<<" info="<<extra_info);
		NOTICE(host<<" "<<port<<" "<<user<<" "<<password<<" "<<database);
        //if(!otlDb::inst()->start(dbkey)){
        if(!myDb::inst()->start(host,port,user,password,database)){
            NOTICE("DB Logon Failed..");
            flag=true;
            break;
        }
    }while(0);
    if(flag){
        NOTICE("Logger&&DB Start Failed...");
        return 0;
    }
	
	std::cout << "Successfully"<<std::endl;
    NOTICE("Logger&&DB Start Successfully...");
#if 0
	std::cout << mapPath<<std::endl;
	if(!mapManagerInstance::inst()->init(mapPath)){
        NOTICE("MapData Init Failed..");
		return 0;	
	}
	//mapManagerInstance::inst()->m_mapManager.reset();
#endif
#if 1
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != res){
		printf("init libcurl failed.");
		curl_global_cleanup();
		return -1;
	}
	if(!mapManagerInstance::inst()->init(true)){
		curl_global_cleanup();
		return 0;
	}
#if 0
	if(!getAndInitMapBigData()){
		curl_global_cleanup();
		return 0;
	}
#endif
	NOTICE("MapBigData Load Successfully~~~~~~~~~~~~~~~~~~");
	provinceListManagerInstance::inst()->createWorkThread(citylist,extra_info);
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(24*60*60));
		//if(mapManagerInstance::inst()->init(false)){
		if(loadDataFromHDFS()){
			if(mapManagerInstance::inst()->init(true)){
				provinceListManagerInstance::inst()->changeProvinceFlag();
			}
		}
	}
	curl_global_cleanup();
		
#endif
#if 0
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(60));
        config_file cf;
        cf.open("./config.ini");
        std::string mPath=cf.get("service","map_path","");
        std::cout << mPath<<std::endl;
        if(mPath != mapPath){
			NOTICE("MapPath Changed......");
            mapPath=mPath;
			if(!mapManagerInstance::inst()->reload(mapPath)){
				NOTICE("MapData Reload Failed..");
				return 0;	
			}
			provinceListManagerInstance::inst()->changeProvinceFlag();
        }
    }
#endif
#if 0
	std::string x="truncate table congest_detail";
    myDb::inst()->tryPut(x);
	getchar();
	const char *sql="insert into congest_detail (congest_id,links,geom_line) values ('%d','12|11z中国',geomfromtext('POINT(11 22)'))";
	char nsql[1024]{0};
	for(int i=0;i<1000;i++){
		std::this_thread::sleep_for(std::chrono::seconds(10));
		snprintf(nsql,1024,sql,i);
        myDb::inst()->tryPut(nsql);
        std::cout <<i<<std::endl;
//		NOTICE("hello world");
	}
    std::cout<<"End"<<std::endl;
    getchar();
#endif
}
