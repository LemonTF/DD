#ifndef __OVERALL_DATA_MANAGEMENT__HPP
#define __OVERALL_DATA_MANAGEMENT__HPP
#include <memory>
#include <map>

class MapManager;
class provincesOfBusiness;

struct mapManagerInstance{
	mapManagerInstance();
	static mapManagerInstance* inst();
	bool init(bool);
	std::shared_ptr<MapManager> m_mapManager;
};
struct provinceListManagerInstance{
	static provinceListManagerInstance * inst();
	void createWorkThread(const std::string &list,const std::string &extra_info);
	void changeProvinceFlag();
	std::map<uint32_t,std::shared_ptr<provincesOfBusiness>> m_provincesList;
};
#endif
