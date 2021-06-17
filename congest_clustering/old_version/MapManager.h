//
//  MapManager.hpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#ifndef MapManager_hpp
#define MapManager_hpp


#include "route_struct.h"
#include "RoadManager.h"
#include "TopoManager.h"
#include "CoorManager.h"

struct roadinfo{
	roadinfo()=default;
	roadinfo(const std::string&n,const std::string& ac):name(n),admincode(ac){}
	std::string name,admincode;
};
// 处理维度是带方向的linkid
class MapManager{
    
public:

    MapManager();
    ~MapManager();
    
    bool Init(std::string& topo_path, std::string& road_path, std::string& coor_path, uint32_t partition_code);
    
    bool ExistsLink(uint64_t linkid_dir);
    
    bool GetSnodeByLinkID(uint64_t linkid_dir, uint64_t& snode);
    bool GetEnodeByLinkID(uint64_t linkid_dir, uint64_t& enode);
    
    bool GetPreLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& pre_links_vec);
    bool GetNextLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& next_links_vec);
    
    bool GetLengthByLinkID(uint64_t linkid_dir, int& len);
    
    bool GetRoadClsByLinkID(uint64_t linkid_dir, int& road_cls);
    
    bool GetLevelByLinkID(uint64_t linkid_dir, int& level);
    
    bool GetKindInfoByLinkID(uint64_t linkid_dir, KindInfo& kind);
    
    bool GetPointsByLinkID(uint64_t linkid_dir, std::vector<PointCoor> & point_vec);
    bool GetKindInfoByLinkIndex(uint64_t link_index, KindInfo& kind);

    bool Test();
	
private:
    TopoManager* topo_manager;
    RoadManager* road_manager;
    CoorManager* coor_manager; 

	//////////////////////////////////////////
	//这块是因为业务需求添加，后续可删除
public:
	void getLinkNameAdmincode(uint64_t roadid,std::string &name,std::string&admincode);
	void makeLinkInfo();
private:
	std::unordered_map<uint64_t,roadinfo> m_linkinfo;
	/////////////////////////////////////////
};




#endif /* MapManager_hpp */
