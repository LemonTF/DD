//
//  RoadManager.cpp
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#include "RoadManager.h"

RoadManager::RoadManager(){
    road_info_list_=NULL;
	total_buf=NULL;
    road_info_size_=0;
}

RoadManager::~RoadManager(){
    if(road_info_list_){
        delete [] road_info_list_;
        road_info_list_=NULL;
        std::cerr<<"destruct road manager"<<std::endl;
    }
	if(total_buf){
		delete total_buf;
		std::cout<<"delete Road total_buf\n";
		total_buf=NULL;
	}
	link_index_map_.clear();
}

bool RoadManager::LoadRoadData(const char* cache_path, uint32_t & partition_code){
    FILE* file = fopen(cache_path, "rb");
	if(file==NULL) return false; fseeko(file, 0, SEEK_END);
    uint64_t file_size = ftello(file);
    fseeko(file, 0, SEEK_SET);
    
    total_buf = new char[file_size];
    if(!total_buf) {
        std::cerr<<"calc: load data malloc failed"<<std::endl;
        return false;
    }
    fread(total_buf, file_size, 1, file);
    fclose(file);
    file = NULL;
    
    uint64_t offset=0;
    char magiccode[32]={0};
    memcpy(magiccode, total_buf, 32);
    offset+=32;
    printf("LoadRoad magiccode:%s\n",magiccode);
    
    uint64_t save_size=0;
    memcpy(&save_size, total_buf+offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    printf("LoadRoad totaloffset:%lu\n",save_size);
    
    uint32_t partition_code_file=0xFFFFFFFF;
    memcpy(&partition_code_file, total_buf+offset, sizeof(uint32_t));
    offset+=sizeof(uint32_t);
    printf("LoadRoad partition_code:%u\n",partition_code_file);
    
    char save_version[32] = {0};
    memcpy(save_version, total_buf + offset, 32);
    offset += 32;
    printf("LoadRoad save_version:%s\n",save_version);
    
    //uint64_t save_size=0;
    //memcpy(&save_size, total_buf+offset, sizeof(uint64_t));
    //offset += sizeof(uint64_t);
    
    uint32_t roadsize=0;
    memcpy(&roadsize, total_buf+offset, sizeof(uint32_t));
    offset+=sizeof(uint32_t);
    printf("LoadRoad roadsize:%u\n",roadsize);
    
    road_info_size_=roadsize;
    
    road_info_list_ = new RoadInfo*[roadsize];
    
    for (uint32_t index=0; index <roadsize; index++){
        road_info_list_[index]=(RoadInfo*)(total_buf+offset);
        offset+=sizeof(RoadInfo);
        link_index_map_[road_info_list_[index]->road_id] = index;
    }
    printf("success load road list\n");
    return true;
}

bool RoadManager::GetLinkIDByLinkIndex(uint32_t link_index, uint64_t& link_id ){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        link_id = road_info->road_id;
        return true;
    }
}

bool RoadManager::GetLinkIndexByLinkID(uint64_t link_id, uint32_t& link_index){
	std::unordered_map<uint64_t, uint32_t>::iterator iter = link_index_map_.find(link_id);
    if(iter == link_index_map_.end()){
        return false;
    }
    else{
        link_index = iter->second;
        return true;
    }
}

bool RoadManager::GetLengthByLinkIndex(uint32_t link_index, int& len){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        len = road_info->length;
        return true;
    }
}

bool RoadManager::GetLengthByLinkID(uint64_t link_id, int& len){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetLengthByLinkIndex(link_index, len);
}

bool RoadManager::GetRoadClsByLinkIndex(uint64_t link_index, int& road_cls){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        road_cls = road_info->road_cls;
        return true;
    }
}

bool RoadManager::GetRoadClsByLinkID(uint64_t link_id, int& road_cls){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetRoadClsByLinkIndex(link_index, road_cls);
}

bool RoadManager::GetLevelByLinkIndex(uint64_t link_index, int& level){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        level = road_info->level;
        return true;
    }
}

bool RoadManager::GetLevelByLinkID(uint64_t link_id, int& level){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetLevelByLinkIndex(link_index, level);
}

bool RoadManager::GetKindInfoByLinkIndex(uint64_t link_index, KindInfo& kind){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        kind = road_info->kind;
        return true;
    }
}

bool RoadManager::GetKindInfoByLinkID(uint64_t link_id, KindInfo& kind){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetKindInfoByLinkIndex(link_index, kind);
}

bool RoadManager::GetFnodeIDByLinkIndex(uint32_t link_index, uint64_t& Fnode_id){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        Fnode_id = road_info->F_node_id;
        return true;
    }
}

bool RoadManager::GetFnodeIDByLinkID(uint64_t link_id, uint64_t& Fnode_id){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetFnodeIDByLinkIndex(link_index, Fnode_id);
}

bool RoadManager::GetTnodeIDByLinkIndex(uint32_t link_index, uint64_t& Tnode_id){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        Tnode_id = road_info->T_node_id;
        return true;
    }
}

bool RoadManager::GetTnodeIDByLinkID(uint64_t link_id, uint64_t& Tnode_id){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetTnodeIDByLinkIndex(link_index, Tnode_id);
}

bool RoadManager::GetForwardTopoIndex(uint32_t link_index, uint32_t& topo_index){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        topo_index =road_info->forward_index;
        return true;
    }
    return true;
}

bool RoadManager::GetBackwardTopoIndex(uint32_t link_index, uint32_t& topo_index){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        topo_index =road_info->backward_index;
    }
    return true;
}

bool RoadManager::GetCoorOffsetByLinkIndex(uint32_t link_index, uint64_t& offset){
    if(!road_info_list_ || link_index>=road_info_size_){
        return false;
    }
    else{
        RoadInfo* road_info=road_info_list_[link_index];
        if(NULL==road_info){
            return false;
        }
        offset = road_info->coor_offset;
    }
    return true;
}

bool RoadManager::GetCoorOffsetByLinkID(uint64_t link_id, uint64_t& offset){
    uint32_t link_index;
    if(!GetLinkIndexByLinkID(link_id, link_index)){
        return false;
    }
    return GetCoorOffsetByLinkIndex(link_index, offset);
}







