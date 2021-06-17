#ifndef __CONGEST_OBJ_HPP
#define __CONGEST_OBJ_HPP
#include <string>
#include <vector>
#include <ctime>
#include <map>
class linksinfo;
struct congestObj{
	congestObj(uint32_t pid,const std::string&);
	std::string m_mversion;
	std::string m_key;
	std::string m_roadName;
	std::string m_centroid;
	std::string m_geomLine;
	std::time_t m_startTime;
	double		m_totalScore=0;
	double		m_trendData=0;
	uint32_t	m_cityid=0;
	uint32_t	m_adminCode=0;
	uint32_t	m_provinceId=0;
	uint32_t	m_orderId=0;
	bool		m_insertDbFlag=false;
	std::vector<linksinfo> m_linklist;

	std::string getkey(){return m_key;}
	std::string getMultiLineString();
	std::string getCentroid(const std::string&);
	std::string getLinksString();
	std::string getLinksString2();
	std::time_t getStarttime(){return m_startTime;}
	void		setLinks(std::vector<linksinfo> &&v);
	bool		addTotalScore();
	uint64_t	getMaxLengthLinkid();
	double		getLength();
	uint64_t	getConfidence();
	double		getAvgSpeed();
	double		getAvgStatus();
	int32_t		getRoadLevel();
	void		getAdmincode();

	void on_open();
	void on_move();
	void on_close();
	uint32_t getAdminCode(){return m_adminCode;}
	uint32_t getCityid(){return m_cityid;}
	uint32_t getProvinceid(){return m_provinceId;}
	uint32_t getOrderid(){return m_orderId;}
	std::string getRoadname(){return m_roadName;}

	void setNameAdmincode(const std::string&name,const std::string&ac){
		m_roadName=name;
		if(!ac.empty()){
			m_adminCode=std::stoi(ac);
			m_cityid=std::stoi(ac.substr(0,4));
		}
	}
private:
	void insertCongestSummary();
	void insertCongestDetail();
	void updateDetailForCongestSummary();
	void CloseCongestSummary();
	void dealCongestTrend();

};
#endif
