#include "business.h"
#include "provinceOfBusiness.h"
#include <iostream>
#include "congestObject.h"
#include "mydb.h"
#include "CommonTools.h"
void mapVersionBus::workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	if(pob->getMapVersionFlag()){
		pob->disableMapVersionFlag();
		char nsql[128]{0};
		const char*sql="insert into congest_map_version(version_str,create_time) values ('%s','%s');";
		snprintf(nsql,128,sql,pob->getMapVersion().c_str(),CommonTools::now2string().c_str());
		myDb::inst()->tryPut(nsql);
	}
}
void mapVersionBus::on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void mapVersionBus::on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void mapVersionBus::on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
//
void tagServiceBus::workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){}
void tagServiceBus::on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void tagServiceBus::on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void tagServiceBus::on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
//
void ttiJobBus::workProcess(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	uint32_t oid=co->getOrderid();
	if(oid==1){
		std::string info = pob->getInfo();	
		uint32_t admin_code=co->getAdminCode();
		uint32_t cityid=co->getCityid();
		uint32_t provinceid = co->getProvinceid();
		std::string code=std::to_string(admin_code)+";";
		if(info.find(code)!=std::string::npos){
			std::string links=co->getLinksString2();
			std::string name=co->getRoadname();
			std::string key=co->getkey();
			std::string mapversion=pob->getMapVersion();

			int job_id = CommonTools::submitTTIJob(links,name,provinceid,cityid,mapversion);
			char nsql[128]{0};
			const char *sql="update congest_summary set tti_job_id =%d where congest_id ='%s';";
			snprintf(nsql,128,sql,job_id,key.c_str());
			myDb::inst()->tryPut(nsql);
		}
	}
}
void ttiJobBus::on_open(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void ttiJobBus::on_move(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
	workProcess(pob,co);
}
void ttiJobBus::on_close(const std::shared_ptr<provincesOfBusiness> &pob,const std::shared_ptr<congestObj> &co){
}

void businessManager::getBusiness(uint32_t type,std::list<business*> &blist){
	for(int i=0;i<32;i++){
		if((type&(1<<i))){
			if(!m_allBus[i])continue;
			blist.push_back(m_allBus[i].get());
		}
	}
}
businessManager::businessManager()
{
	m_allBus.resize(32);
	m_allBus[0].reset(new mapVersionBus());
	m_allBus[1].reset(new tagServiceBus());
	m_allBus[2].reset(new ttiJobBus());
}
businessManager* businessManager::inst(){
	static businessManager bm;
	return &bm;
};
