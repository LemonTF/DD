#ifndef _PROVINCE_OF_BUSINESS_HPP
#define _PROVINCE_OF_BUSINESS_HPP
#include <memory>
#include <list>
#include <thread>
#include <vector>
#include <map>
#include "route_struct.h"
class business;
class congestObj;
struct linksinfo{
	linksinfo(){
		linkid=
		confidence=
		congest_level=
		road_level=
		length=0;
		speed=0;
		begin_time=time(0);
	}
	linksinfo(uint64_t link,uint32_t conf,uint32_t cl,int32_t level,double v)
		:linkid(link)
		 ,confidence(conf)
		 ,congest_level(cl)
		 ,road_level(level)
		 ,speed(v)
	{
		begin_time=time(0);
	}
	void set(uint32_t conf,uint32_t cl,double v){
		confidence=conf;congest_level=cl;speed=v;
	}
	uint64_t getLengthLevel()const{
		return road_level*length;
	}
	std::string getLinkPointString()const;
	void setuid(std::string &uid){uuid=uid;}
	uint64_t linkid;
	uint32_t confidence;
	uint32_t congest_level;
	int32_t road_level;
	int32_t length;
	double speed;
	std::time_t begin_time;
	std::string uuid;
	std::vector<PointCoor> points;
};
class MapManager;
struct provincesOfBusiness:std::enable_shared_from_this<provincesOfBusiness>
{
	provincesOfBusiness(uint32_t id,uint32_t type,const std::string &data);
	~provincesOfBusiness();
	uint32_t m_id;//省份id
	uint32_t m_type;
	bool m_bFlag;
	bool m_mapUpdateFlag;
	bool m_mapChangedFlag;
	std::string m_info;
	std::string m_mapVersion;
	void changeMapFlag(){m_mapChangedFlag=true;}
	std::string getMapVersion(){return m_mapVersion;}
	void disableMapVersionFlag(){m_mapUpdateFlag=false;}
	bool getMapVersionFlag(){return m_mapUpdateFlag;}
	std::string getInfo(){return m_info;}
private:
	void run();
	bool getTrafficStatusData(const time_t);
	void makeCongestClustering();
	void getUidAndMakeCongestObj(std::vector<std::string> &uall,const std::vector<uint64_t> &vlinks,int size,const std::string &uid);
	void congestClusteringProcess(std::vector<std::string> &ulist);
	std::vector<uint64_t> subClustering(uint64_t,int);

	void preocessBeforeAction(const std::string &csid);
	std::list<business*> m_blist;
	std::unique_ptr<std::thread> m_pThread;
	std::shared_ptr<MapManager> m_mapTool;
	/////
	std::vector<uint64_t>		m_linksList;
	std::map<uint64_t,linksinfo> m_linksInfoList;
	///
	std::vector<std::string>   m_congestList;
	std::map<std::string,std::shared_ptr<congestObj>> m_congestObjList;
};
#endif
