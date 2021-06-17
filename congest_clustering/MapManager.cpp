#include "MapManager.h"
MapManager::MapManager()
{
	m_map.reset(new didi::f3::mapdb::MapManager(didi::f3::kUseMemCache | didi::f3::kMapShared));
}

bool MapManager::Init(bool setflag){
	if(F3_DATA_CHANGED==m_map->LoadData("../etc/mapdata",0,setflag))
		return true;
	return false;
}

bool MapManager::ExistsLink(uint64_t linkid_dir){
	return m_map->ExistsLink(linkid_dir);
}

bool MapManager::GetLengthByLinkID(uint64_t linkid_dir, int& len){
	return m_map->GetLengthByLinkID(linkid_dir,len);
}

bool MapManager::GetRoadClsByLinkID(uint64_t linkid_dir, int& road_cls){
	return m_map->GetRoadClsByLinkID(linkid_dir,road_cls);
}

bool MapManager::GetLevelByLinkID(uint64_t linkid_dir, int& level){
	return m_map->GetLevelByLinkID(linkid_dir,level);
}

bool MapManager::GetPointsByLinkID(uint64_t linkid_dir, std::vector<PointCoor> & point_vec){
	std::string geom;
	bool ret=m_map->GetPointsByLinkID(linkid_dir,geom);
	if(ret){
		std::istringstream input(geom);
		int i=0;
		uint32_t x,y;
		for(std::string line;std::getline(input,line,',');){
			++i;
			if(i%2==1){x=std::stoul(line);}
			else if (i%2==0){
				y=std::stoul(line);
				point_vec.emplace_back(x,y);
			}
		}
	}
	return ret;
}

bool MapManager::GetSnodeByLinkID(uint64_t linkid_dir, uint64_t& snode){
	return m_map->GetSnodeByLinkID(linkid_dir,snode);
}

bool MapManager::GetEnodeByLinkID(uint64_t linkid_dir, uint64_t& enode){
	return m_map->GetEnodeByLinkID(linkid_dir,enode);
}

bool MapManager::GetPreLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& pre_links_vec){
	bool ret=false;
	std::string result;
	ret = m_map->GetPreLinksByLinkID(linkid_dir,result);
	if(ret){
		std::istringstream input(result);
		for(std::string line;std::getline(input,line,',');){
			pre_links_vec.push_back(std::stoul(line));
		}
	}
	return ret;
}

bool MapManager::GetNextLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& next_links_vec){
	bool ret=false;
	std::string result;
	ret = m_map->GetNextLinksByLinkID(linkid_dir,result);
	if(ret){
		std::istringstream input(result);
		for(std::string line;std::getline(input,line,',');){
			next_links_vec.push_back(std::stoul(line));
		}
	}
	return ret;
}
