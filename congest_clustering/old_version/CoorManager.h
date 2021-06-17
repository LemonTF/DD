//
//  CoorManager.hpp
//  thriftDemo
//
//  Created by LiuGuoping on 16/12/15.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#ifndef CoorManager_hpp
#define CoorManager_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include "route_struct.h"

class CoorManager{
public:
    CoorManager();
    ~CoorManager();
    
    bool LoadCoorData(const char* cache_path, uint32_t & partition_code);
    bool GetRoadPoints(uint64_t coor_offset, std::vector<PointCoor> & point_vec, bool reverse=false);
    
private:
    char *coor_body_buf_;
	char *total_buf;
};

#endif /* CoorManager_hpp */

