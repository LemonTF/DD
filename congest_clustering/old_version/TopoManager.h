//
//  TopoManager.hpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#ifndef TopoManager_hpp
#define TopoManager_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include "route_struct.h"


class TopoManager{
public:
    TopoManager();
    ~TopoManager();
    
    //bool Init();
    bool LoadTopoData(const char* cache_path, uint32_t & partition_code);
    
    bool GetLinkIndex(uint32_t topo_index, uint32_t& link_index);
    
    bool GetPreTopoIndexs(uint32_t topo_index, std::vector<uint32_t>& pre_topo_indexs);
    bool GetNextTopoIndexs(uint32_t topo_index, std::vector<uint32_t>& next_topo_indexs);
    
    bool GetPreLinkIndexs(uint32_t topo_index, std::vector<uint32_t>& pre_link_indexs);
    bool GetNextLinkIndexs(uint32_t topo_index, std::vector<uint32_t>& next_link_indexs);
    
    bool GetDirection(uint32_t topo_index, int& dir);
    
    
private:
//    TOPOINFO **topo_info_list_;
    TOPOINFO *topo_info_list_;
    uint32_t topo_info_size_;
	char * total_buf;
};

#endif /* TopoManager_hpp */
