#include "congestObject.h"
#include <uuid/uuid.h>
#include "provinceOfBusiness.h"
#include "CommonTools.h"
#include "mydb.h"
#include "LogUtils.h"
#include <geos.h>
#include <geos/algorithm/InteriorPointLine.h>
congestObj::congestObj(uint32_t pid,const std::string &v)
	:m_mversion(v)
	,m_provinceId(pid)
{
	uuid_t u;
	m_startTime=time(0);
	char szUUID[37]{0};
	uuid_generate(u);
	uuid_unparse_lower(u, szUUID);
	m_key = szUUID;
}

void congestObj::setLinks(std::vector<linksinfo> &&v){
	m_linklist=std::move(v);
}
uint64_t congestObj::getMaxLengthLinkid(){
	uint64_t lengthLevel=0;
	uint64_t linkid=0;
	for(const auto &info:m_linklist){
		uint64_t ll=info.getLengthLevel();
		if(ll>lengthLevel){
			lengthLevel=ll;
			linkid=info.linkid/10;
		}
	}
	return linkid;
}
double congestObj::getLength(){
	double lt=0;
	float weight_road_level=1.0;
	float weight_congest_level=1.0;
	for(const auto &info:m_linklist){
		if(info.road_level==1)
			weight_road_level=1.1;
		else if(info.road_level==2)
			weight_road_level=1.0;
		else if(info.road_level==3)
			weight_road_level=0.85;

		if(info.congest_level==4){
			weight_congest_level=1.2;
		}else if(info.congest_level==3){
			weight_congest_level=0.8;
		}
		lt+=info.length*weight_congest_level*weight_road_level;
	}
	return lt;
}
bool congestObj::addTotalScore(){
	m_totalScore+=getLength();
	return m_totalScore>1000;
}
std::string congestObj::getMultiLineString(){
	std::stringstream ss;
	ss<<"MULTILINESTRING (";
	for(const auto &info:m_linklist){
		ss<<info.getLinkPointString()<<",";
	}
	std::string s=ss.str();
	s.pop_back();
	s+=")";
	return s;
}
std::string congestObj::getCentroid(const std::string &lineString){
	geos::io::WKTReader reader;
    auto gm=reader.read(lineString);
    geos::algorithm::InteriorPointLine ipl(gm.get());
	geos::geom::Coordinate c;
    ipl.getInteriorPoint(c);
	std::stringstream ss;
	ss<<c.x<<","<<c.y;
	return ss.str();
}
uint64_t congestObj::getConfidence(){
	uint64_t con=0;
	for(const auto &info:m_linklist){
		con+=info.confidence;
	}
	return con/m_linklist.size();
}
std::string congestObj::getLinksString(){
	std::stringstream ss;
	for(const auto &info:m_linklist){
		ss<<info.linkid<<"|"<<info.congest_level<<"|"<<info.length<<"|"<<info.road_level<<",";
	}
	std::string s=ss.str();
	s.pop_back();
	return s;
}
std::string congestObj::getLinksString2(){
	std::stringstream ss;
	for(const auto &info:m_linklist){
		ss<<info.linkid<<",";
	}
	std::string s=ss.str();
	s.pop_back();
	return s;
}
double congestObj::getAvgSpeed(){
	double spd=0.0;
	for(const auto &info:m_linklist){
		spd+=1.0/info.speed;
	}
	double avspd = m_linklist.size()/spd;
	return int(avspd*100)/100.0;
}
double congestObj::getAvgStatus(){
	uint32_t sc=0;
	for(const auto &info:m_linklist){
		sc+=info.congest_level;
	}
	return sc*1.0/(m_linklist.size()*1.0);
}
int32_t congestObj::getRoadLevel(){
	std::map<int32_t,uint64_t> roadL;
	for(const auto &info:m_linklist){
		roadL[info.road_level]+=info.length;
	}
	int32_t level=0;uint64_t len=0;
	for(const auto &p:roadL){
		if(p.second>len){
			len=p.second;
			level=p.first;
		}
	}
	return level;
}
#if 0
void congestObj::getAdmincode(){
	std::string poi;
	{
		logEplsTimeout et("GetRoadNameByLinkID");
		std::string lineString=getMultiLineString();
		std::string disp=getCentroid(lineString);
		poi = CommonTools::GetPoiDesc(disp);
	}
	if(m_roadName.empty()){
		m_roadName="无名道路";
	}
	m_roadName+=";"+poi;
}
#endif
#if 1
void congestObj::getAdmincode(){
	uint64_t linkid=getMaxLengthLinkid();
	std::string roadname,admincode;
	std::string poi;
	{
		logEplsTimeout et("GetRoadNameByLinkID");
		std::string lineString=getMultiLineString();
		//std::cout <<lineString<<std::endl;
		std::string disp=getCentroid(lineString);
		//roadname=CommonTools::GetRoadNameByLinkID(linkid,m_mversion);
		CommonTools::getAdmincode(linkid,roadname,admincode);
		poi = CommonTools::GetPoiDesc(disp);
	}
	if(roadname.empty()){
		m_roadName="无名道路";
	}else{
		m_roadName=roadname;
	}
	m_roadName+=";"+poi;
	if(!admincode.empty()){
		m_adminCode=std::stoi(admincode);
		m_cityid=std::stoi(admincode.substr(0,4));
	}
	NOTICE("CC:"<<m_key<<"@"<<m_provinceId<<"@"<<m_roadName<<"@"<<m_adminCode<<"@"<<m_cityid);
}
#endif
void congestObj::insertCongestSummary(){	
	m_insertDbFlag=true;
	getAdmincode();
	char nsql[2048]{0};
	const char *sql="insert into congest_summary(congest_id,start_time,name,create_time,update_time,realtime_detail_id,total_score,cityid,admin_code,province_id) values('%s','%s','%s',now(),now(),%d,%.2f,%d,%d,%d);";
	snprintf(nsql,2048,sql,m_key.c_str(),CommonTools::unixtime2str(m_startTime).c_str(),m_roadName.c_str(),m_orderId,m_totalScore,m_cityid,m_adminCode,m_provinceId);
	myDb::inst()->tryPut(nsql);
}
void congestObj::insertCongestDetail(){
	std::string linkstring=getLinksString();
	std::string lineString=getMultiLineString();
	std::string disp=getCentroid(lineString);
	int32_t level=getRoadLevel();
	double len=getLength();
	double spd=getAvgSpeed();
	double status=getAvgStatus();
	uint64_t con=getConfidence();
	std::stringstream ss;
	ss<<"insert into congest_detail(congest_id,links,display_point,geom_line,dtime,order_id,road_level,weight_length,total_score,admin_code,avg_spd,avg_status,confindence,create_time) values('"<<m_key<<"','"<<linkstring<<"','"<<disp<<"',geomfromtext('"<<lineString<<"'),'"<<CommonTools::now2string(1)<<"',"<<m_orderId<<","<<level<<","<<len<<","<<m_totalScore<<","<<m_adminCode<<","<<spd<<","<<status<<","<<con<<",'"<<CommonTools::now2string()<<"');";
	myDb::inst()->tryPut(ss.str());
#if 0
	char nsql[8192]{0};
	const char*sql =" insert into congest_detail(congest_id,links,display_point,geom_line,dtime,order_id,road_level,weight_length,total_score,admin_code,avg_spd,avg_status,confindence) values ('%s','%s','%s',geomfromtext('%s'),'%s',%d,%d,%.2f,%.2f,%d,%.2f,%.2f,%lu);";
	snprintf(nsql,8192,sql,m_key.c_str(),linkstring.c_str(),disp.c_str(),lineString.c_str(),CommonTools::now2string().c_str(),m_orderId,level,len,m_totalScore,m_adminCode,spd,status,con);
	myDb::inst()->tryPut(nsql);
#endif
}
void congestObj::updateDetailForCongestSummary(){
	char nsql[512]{0};
	const char*sql="update congest_summary set realtime_detail_id = %d,total_score=%.2f,update_time='%s' where congest_id = '%s';";
	snprintf(nsql,512,sql,m_orderId,m_totalScore,CommonTools::now2string().c_str(),m_key.c_str());
	myDb::inst()->tryPut(nsql);
}
void congestObj::CloseCongestSummary(){
	char nsql[512]{0};
	const char*sql="update congest_summary set closed=1,close_type=1,end_time='%s' where congest_id = '%s';";
	snprintf(nsql,512,sql,CommonTools::now2string().c_str(),m_key.c_str());
	myDb::inst()->tryPut(nsql);
}
void congestObj::dealCongestTrend(){
	if(m_orderId%5==1)m_trendData=getLength();
	if(m_orderId%5==0){
		double avg_spd=(getLength()-m_trendData)/4/120*3.6;
		int trend_type = 0;
		if (avg_spd > 2){
			trend_type = 1;
		}else if(avg_spd<-2){
			trend_type = 2;
		}
		char nsql[1024]{0};
		sprintf(nsql,"UPDATE congest_summary SET trend_type = '%d', realtime_trend_id = %d WHERE congest_id = '%s';",trend_type, m_orderId/5,m_key.c_str());
		myDb::inst()->tryPut(nsql);

		memset(nsql,0,1024);
		sprintf(nsql,"INSERT INTO congest_trend(trend_type,trend_acc_value,create_time,congest_id,order_id,detail_order_id) VALUES ('%d','%.2f','%s','%s',%d,'%d')",trend_type,avg_spd,CommonTools::now2string().c_str(),m_key.c_str(),m_orderId/5,m_orderId);
		myDb::inst()->tryPut(nsql);
	}
}
void congestObj::on_open(){
	//add total socre
	if(!addTotalScore())return;
	++m_orderId;

	insertCongestSummary();
	insertCongestDetail();
}

void congestObj::on_move(){
	bool f = addTotalScore();
	if(!m_insertDbFlag && !f) return;
	++m_orderId;
	if(m_insertDbFlag)
		updateDetailForCongestSummary();
	else
		insertCongestSummary();
	insertCongestDetail();
	dealCongestTrend();
}
void congestObj::on_close(){
	if(m_insertDbFlag)
		CloseCongestSummary();
}
