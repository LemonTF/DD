//
//  RoadManager.hpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#ifndef RoadManager_hpp
#define RoadManager_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "route_struct.h"

class RoadManager{
public:
    RoadManager();
    ~RoadManager();
    
    bool LoadRoadData(const char* cache_path, uint32_t & partition_code);
    
    bool GetLinkIDByLinkIndex(uint32_t link_index, uint64_t& link_id);
    bool GetLinkIndexByLinkID(uint64_t link_id, uint32_t& link_index);
    
    bool GetLengthByLinkID(uint64_t link_id, int& len);
    bool GetLengthByLinkIndex(uint32_t link_index, int& len);
    
    bool GetRoadClsByLinkID(uint64_t link_id, int& road_cls);
    bool GetRoadClsByLinkIndex(uint64_t link_index, int& road_cls);
    
    bool GetKindInfoByLinkID(uint64_t link_id, KindInfo& kind);
    bool GetKindInfoByLinkIndex(uint64_t link_index, KindInfo& kind);
    
    bool GetLevelByLinkID(uint64_t link_id, int& level);
    bool GetLevelByLinkIndex(uint64_t link_index, int& level);
    
    bool GetFnodeIDByLinkID(uint64_t link_id, uint64_t& Fnode_id);
    bool GetFnodeIDByLinkIndex(uint32_t link_index, uint64_t& Fnode_id);
    
    bool GetTnodeIDByLinkID(uint64_t link_id, uint64_t& Tnode_id);
    bool GetTnodeIDByLinkIndex(uint32_t link_index, uint64_t& Tnode_id);
    
    bool GetForwardTopoIndex(uint32_t link_index, uint32_t& topo_index);
    bool GetBackwardTopoIndex(uint32_t link_index, uint32_t& topo_index);
    
    bool GetCoorOffsetByLinkID(uint64_t link_id, uint64_t& offset);
    bool GetCoorOffsetByLinkIndex(uint32_t link_index, uint64_t& offset);     
    
private:
    RoadInfo **road_info_list_;
    uint32_t road_info_size_;
	char *total_buf;
public: 
    std::unordered_map<uint64_t, uint32_t> link_index_map_;
};

#endif /* RoadManager_hpp */
