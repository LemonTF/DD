#include "overallDataManagement.h"
#include "MapManager.h"
#include "LogUtils.h"
#include "provinceOfBusiness.h"
#include "unistd.h"
mapManagerInstance::mapManagerInstance(){
	//m_mapManager=std::make_shared<MapManager>();
}
mapManagerInstance* mapManagerInstance::inst(){
	static mapManagerInstance mi;	
	return &mi;
}
bool mapManagerInstance::init(bool flag){
	if(!flag){
		if(!m_mapManager)return false;
	}else{
		m_mapManager.reset(new MapManager);
	}
	return m_mapManager->Init(flag);
}

provinceListManagerInstance * provinceListManagerInstance::inst(){
	static provinceListManagerInstance  pli;
	return &pli;
}
void provinceListManagerInstance::createWorkThread(const std::string &list,const std::string &extra_info){
	std::istringstream input(list);
	for(std::string line;std::getline(input,line,',');){
		std::vector<uint32_t> vu;
		std::istringstream iss(line);
		for(std::string item;std::getline(iss,item,':');){
			vu.push_back(std::stoi(item));
		}
		NOTICE("CreateWorkThread:"<<vu[0]<<",type="<<vu[1]);
		m_provincesList[vu[0]]=std::make_shared<provincesOfBusiness>(vu[0],vu[1],extra_info);
	}
}
void provinceListManagerInstance::changeProvinceFlag(){
	for(const auto &p:m_provincesList){
		p.second->changeMapFlag();
	}
}
