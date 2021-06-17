//
//  MapManager.cpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#include "MapManager.h"
#include "CommonTools.h"

MapManager::MapManager(){
    coor_manager = NULL;
    topo_manager = NULL;
    road_manager = NULL;
}

MapManager::~MapManager(){
    if(coor_manager){
        delete coor_manager;
		coor_manager=NULL;
    }
    if(topo_manager){
        delete topo_manager;
        topo_manager=NULL;
    }
    if(road_manager){
        delete road_manager;
        road_manager=NULL;
    }
    std::cerr<<"destruct map manager"<<std::endl;
}

bool MapManager::Init(std::string& topo_path, std::string& road_path, std::string& coor_path,uint32_t partition_code){
    
    topo_manager = new TopoManager();
    road_manager = new RoadManager();
    coor_manager = new CoorManager();
    if(!topo_manager || !road_manager || !coor_manager){
        std::cerr<<"map manager init error!"<<std::endl;
        return false;
    }
    if(!coor_manager->LoadCoorData(coor_path.c_str(), partition_code)){
        std::cerr<<"coor manager load data error!"<<std::endl;
        return false;
    }

    if(!topo_manager->LoadTopoData(topo_path.c_str(), partition_code)){
        std::cerr<<"topo manager load data error!"<<std::endl;
        return false;
    }
    if(!road_manager->LoadRoadData(road_path.c_str(), partition_code)){
        std::cerr<<"road manager load data error!"<<std::endl;
        return false;
    }
    std::cout<<"map manager init success!"<<std::endl;

    return true;
}

bool MapManager::ExistsLink(uint64_t linkid_dir){
    uint64_t root_linkid=linkid_dir/10;
    int direction=linkid_dir % 10;
    if(direction != 0 && direction!=1){
        return false;
    }
    uint32_t link_index;
    if(!road_manager->GetLinkIndexByLinkID(root_linkid,link_index)){
        return false;
    }
    //check direction
    if(direction==0){
        uint32_t topo_index=0;
        if(!road_manager->GetForwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
    }
    else if(direction==1){
        uint32_t topo_index=0;
        if(!road_manager->GetBackwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get backward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
    }
    else{
        //dir error
        return false;
    }
    return true;
}

bool MapManager::GetLengthByLinkID(uint64_t linkid_dir, int& len){
    uint64_t root_linkid=linkid_dir/10;
    return road_manager->GetLengthByLinkID(root_linkid, len);
}

bool MapManager::GetKindInfoByLinkIndex(uint64_t linkid_dir, KindInfo& kind){
    uint64_t root_linkid=linkid_dir/10;
    return road_manager->GetKindInfoByLinkIndex(root_linkid, kind);
}

bool MapManager::GetRoadClsByLinkID(uint64_t linkid_dir, int& road_cls){
    uint64_t root_linkid=linkid_dir/10;
    return road_manager->GetRoadClsByLinkID(root_linkid, road_cls);
}

bool MapManager::GetLevelByLinkID(uint64_t linkid_dir, int& level){
    uint64_t root_linkid=linkid_dir/10;
    return road_manager->GetLevelByLinkID(root_linkid, level);
}

bool MapManager::GetKindInfoByLinkID(uint64_t linkid_dir, KindInfo& kind){
    uint64_t root_linkid=linkid_dir/10;
    return road_manager->GetKindInfoByLinkID(root_linkid, kind);
}

bool MapManager::GetPointsByLinkID(uint64_t linkid_dir, std::vector<PointCoor> & point_vec){
    uint64_t root_linkid=linkid_dir/10;
    bool reverse = (linkid_dir%10 == 1);
    uint64_t coor_offset;
    if(!road_manager->GetCoorOffsetByLinkID(root_linkid, coor_offset)){
        return false;
    }
    return coor_manager->GetRoadPoints(coor_offset, point_vec, reverse);
}

bool MapManager::GetSnodeByLinkID(uint64_t linkid_dir, uint64_t& snode){
    uint64_t root_linkid=linkid_dir/10;
    int direction=linkid_dir % 10;
    if(direction != 0 && direction!=1){
        return false;
    }
    uint32_t link_index;
    if(!road_manager->GetLinkIndexByLinkID(root_linkid,link_index)){
        return false;
    }
    //check direction
    if(direction==0){
        uint32_t topo_index=0;
        if(!road_manager->GetForwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
        //get fnode
        if(!road_manager->GetFnodeIDByLinkID(root_linkid, snode)){
            return false;
        }
    }
    else{
        uint32_t topo_index=0;
        if(!road_manager->GetBackwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
        //get tnode
        if(!road_manager->GetTnodeIDByLinkID(root_linkid, snode)){
            return false;
        }
    }
    return true;
}

bool MapManager::GetEnodeByLinkID(uint64_t linkid_dir, uint64_t& enode){
    uint64_t root_linkid=linkid_dir/10;
    int direction=linkid_dir % 10;
    if(direction != 0 && direction!=1){
        return false;
    }
    uint32_t link_index;
    if(!road_manager->GetLinkIndexByLinkID(root_linkid,link_index)){
        return false;
    }
    //check direction
    if(direction==0){
        uint32_t topo_index=0;
        if(!road_manager->GetForwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
        //get fnode
        if(!road_manager->GetTnodeIDByLinkID(root_linkid, enode)){
            return false;
        }
    }
    else{
        uint32_t topo_index=0;
        if(!road_manager->GetBackwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
        if(0xFFFFFFFF==topo_index){
            return false;
        }
        //get tnode
        if(!road_manager->GetFnodeIDByLinkID(root_linkid, enode)){
            return false;
        }
    }
    return true;
}

bool MapManager::GetPreLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& pre_links_vec){
    uint64_t root_linkid=linkid_dir/10;
    int direction=linkid_dir % 10;
    if(direction != 0 && direction!=1){
        return false;
    }
    uint32_t link_index;
    if(!road_manager->GetLinkIndexByLinkID(root_linkid,link_index)){
        return false;
    }
    
    uint32_t topo_index=0;
    if(direction==0){
        if(!road_manager->GetForwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
    }
    else{
        if(!road_manager->GetBackwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
    }
    if(0xFFFFFFFF==topo_index){
        return false;
    }
    //according topo index to query
    std::vector<uint32_t> pre_topo_indexs;
    if(!topo_manager->GetPreTopoIndexs(topo_index, pre_topo_indexs)){
        return false;
    }
    for (std::size_t i=0; i<pre_topo_indexs.size(); i++){
        uint32_t pre_topo_index = pre_topo_indexs[i];
        //get link index
        uint32_t pre_link_index;
        if(!topo_manager->GetLinkIndex(pre_topo_index, pre_link_index)){
            return false;
        }
        uint64_t pre_linkid_nodir;
        if(!road_manager->GetLinkIDByLinkIndex(pre_link_index, pre_linkid_nodir)){
            return false;
        }
        int dir;
        if(!topo_manager->GetDirection(pre_topo_index, dir)){
            return false;
        }
        uint64_t pre_linkid_dir = pre_linkid_nodir * 10 + dir;
        pre_links_vec.push_back(pre_linkid_dir);
    }
    return true;
}

bool MapManager::GetNextLinksByLinkID(uint64_t linkid_dir, std::vector<uint64_t>& next_links_vec){
    uint64_t root_linkid=linkid_dir/10;
    int direction=linkid_dir % 10;
    if(direction != 0 && direction!=1){
        return false;
    }
    uint32_t link_index;
    if(!road_manager->GetLinkIndexByLinkID(root_linkid,link_index)){
        return false;
    }
    
    uint32_t topo_index=0;
    if(direction==0){
        if(!road_manager->GetForwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
    }
    else{
        if(!road_manager->GetBackwardTopoIndex(link_index, topo_index)){
            std::cerr<<"error can not get forward topoindex"<<std::endl;
            return false;
        }
    }
    if(0xFFFFFFFF==topo_index){
        return false;
    }
    //according topo index to query
    std::vector<uint32_t> next_topo_indexs;
    if(!topo_manager->GetNextTopoIndexs(topo_index, next_topo_indexs)){
        return false;
    }
    for (std::size_t i=0; i<next_topo_indexs.size(); i++){
        uint32_t next_topo_index = next_topo_indexs[i];
        //get link index
        uint32_t next_link_index;
        if(!topo_manager->GetLinkIndex(next_topo_index, next_link_index)){
            return false;
        }
        uint64_t next_linkid_nodir;
        if(!road_manager->GetLinkIDByLinkIndex(next_link_index, next_linkid_nodir)){
            return false;
        }
        int dir;
        if(!topo_manager->GetDirection(next_topo_index, dir)){
            return false;
        }
        uint64_t next_linkid_dir = next_linkid_nodir * 10 + dir;
        next_links_vec.push_back(next_linkid_dir);
    }
    return true;

}

bool MapManager::Test(){
    uint64_t linkid_array[] = {282031810};
    int testsize = sizeof(linkid_array)/sizeof(linkid_array[0]);
    
    //std::cout<<"test size is :"<<testsize<<std::endl;
    for (int link_loop=0; link_loop<testsize; link_loop++){
        uint64_t linkid = linkid_array[link_loop];
        int length;
        GetLengthByLinkID(linkid, length);
        
        uint64_t snode,enode;
        GetSnodeByLinkID(linkid,snode);
        GetEnodeByLinkID(linkid,enode);
        
        std::vector<uint64_t> pre_link_vec;
        GetPreLinksByLinkID(linkid, pre_link_vec);
        std::vector<uint64_t> next_link_vec;
        GetNextLinksByLinkID(linkid,next_link_vec);
        
        //cout
        std::cout<<"linkid: "<<linkid<<" length: "<<length<<" snode: "<<snode<<" enode: "<<enode<<std::endl;
        for (std::size_t i=0; i<pre_link_vec.size(); i++){
            std::cout<<"pre: "<<pre_link_vec[i]<<std::endl;
        }
        for (std::size_t i=0; i<next_link_vec.size(); i++){
            std::cout<<"next: "<<next_link_vec[i]<<std::endl;
        }
        
    }
    
    return true;
}
/////////////////////////////------------------------------
void MapManager::makeLinkInfo(){
	for(const auto &p:road_manager->link_index_map_){
		std::string name,ac;
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		CommonTools::getAdmincode(p.first,name,ac);
//		NOTICE("id:"<<p.first<<":"<<name<<":"<<ac);
		m_linkinfo.insert({p.first,{name,ac}});
	}
}
void MapManager::getLinkNameAdmincode(uint64_t roadid,std::string &name,std::string&admincode){
	auto ri=m_linkinfo[roadid];
	name=ri.name;admincode=ri.admincode;
}
