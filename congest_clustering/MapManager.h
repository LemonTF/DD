#ifndef MapManager_hpp
#define MapManager_hpp
#include "route_struct.h"
#include "mapdb/mapmanager.hpp"

class MapManager{
public:
	MapManager();
	bool Init(bool);
    
    bool ExistsLink(uint64_t linkid_dir);
    
    bool GetSnodeByLinkID(uint64_t linkid_dir, uint64_t& snode);
    bool GetEnodeByLinkID(uint64_t linkid_dir, uint64_t& enode);
    
    bool GetPreLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& pre_links_vec);
    bool GetNextLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& next_links_vec);
    
    bool GetLengthByLinkID(uint64_t linkid_dir, int& len);
    bool GetRoadClsByLinkID(uint64_t linkid_dir, int& road_cls);
    bool GetLevelByLinkID(uint64_t linkid_dir, int& level);
    
    bool GetPointsByLinkID(uint64_t linkid_dir, std::vector<PointCoor> & point_vec);
	std::shared_ptr<didi::f3::mapdb::MapManager> m_map;
};
#endif /* MapManager_hpp */
