//
//  route_struct.h
//  cluster
//
//  Created by LiuGuoping on 16/11/30.
//  Copyright © 2016年 LiuGuoping. All rights reserved.
//

#ifndef route_struct_h
#define route_struct_h
#include <stdint.h>
#include <string.h>
#include <iostream>
enum NavigationAction {
    MainAction_Continue=0,        //直行
    MainAction_Slight_Right=1,    //右前方
    MainAction_Turn_Right=2,      //右转
    MainAction_Turn_Hard_Right=3, //右后转
    MainAction_UTurn=4,           //左转调头
    MainAction_Turn_Hard_Left=5,  //左后转
    MainAction_Turn_Left=6,       //左转
    MainAction_Slight_Left=7,     //左前方
    MainAction_NULL=8             //顺行
};

// link header struct
struct LinkOffsetInfo{
    uint64_t link_id;
    uint64_t offset;        //该link对应的相对于linkbody起始位置的偏移量
};

// 道路属性
struct KindInfo {
    uint32_t none :1;                    // 0x01 无属性
    uint32_t seperate_link :1;           // 0x02 上下线分离
    uint32_t intersection_link :1;       // 0x04 交叉点内Link
    uint32_t ic :1;                      // 0x05 ic
    uint32_t jct :1;                     // 0x03 jct
    uint32_t tunnel :1;                  // 0x0f 隧道
    uint32_t service_area :1;            // 0x07 服务区
    uint32_t parking_area :1;            // 0x06 停车区
    uint32_t side_link :1;               // 0x0a 辅路
    uint32_t roundabout :1;              // 0x00 环岛
    uint32_t bridge :1;                  // 0x08 桥
    uint32_t walking_link :1;            // 0x09 步行街
    uint32_t ramp :1;                    // 0x0b 匝道
    uint32_t closed_link :1;             // 0x0c 全封闭道路
    uint32_t undefine_area :1;           // 0x0d 未定义交通区域
    uint32_t poi_connected_link :1;      // 0x0e POI连接路
    uint32_t bus_lane :1;                // 0x11 公交专用道
    uint32_t right_ahead_link :1;        // 0x12 提前右转
    uint32_t left_ahead_link :1;         // 0x15 提前左转
    uint32_t scenic_link :1;             // 0x13 风景路线
    uint32_t inside_region_link :1;      // 0x14 区域内道路
    uint32_t u_turn :1;                  // 0x16 掉头口
    uint32_t main_side_port_link :1;     // 0x17 主辅路出入口
    uint32_t parking_guide_link :1;      // 0x19 停车位引导路
    uint32_t virtual_link :1;            // 0x18 虚拟连接路
    uint32_t parking_port_link :1;       // 0x1a 停车场出入口连接路
    uint32_t reserve : 6;                // 预留 默认为0
};

struct RoadInfo{
    uint64_t road_id;
    uint64_t coor_offset;
    uint32_t F_node_id;
    uint32_t T_node_id;
    
    uint32_t  forward_index;// 正向拓扑
    uint32_t  backward_index;// 反向拓扑
    
    uint32_t road_name_offset;
    uint16_t length;                 	//link长度
    uint16_t level : 4;               	//阈值划分等级
    uint16_t road_cls : 4;         		//道路等级
    uint16_t spd_cls : 4;             	//限速等级
    uint16_t display_layer : 4;         	//显示层级
    
    KindInfo kind;                  		//属性
    uint32_t s_exist_light : 2;      	//0:无红绿灯1:简单红绿灯 2:复杂红绿灯
    uint32_t e_exist_light : 2;      	//0:无红绿灯1:简单红绿灯 2:复杂红绿灯
    uint32_t s_exist_toll : 1;          	//0:非收费站 1:收费站
    uint32_t e_exist_toll : 1;          	//0:非收费站 1:收费站
    uint32_t s_complex_node : 1;      	//0:简单路口 1:复杂路口
    uint32_t e_complex_node : 1;         //0:简单路口 1:复杂路口
    uint32_t publish_speed : 1;          //0:不发布速度 1:发布速度
    uint32_t publish_state : 1;          //0:不渲染状态 1:渲染状态
    uint32_t reserve : 22;
};

struct TopoLink {
    uint32_t topo_index;              	//入度或出度link的索引
    uint32_t navi_action : 8;          	//导航动作
    uint32_t weight : 8;
    uint32_t RuleType : 16;
};

struct Topo {
public:
    TopoLink *ListI() { return (TopoLink *)(this + 1); }
    TopoLink *ListO() { return (TopoLink *)(this + 1) + in_link_cnt; }
    uint32_t road_index;
    uint32_t weight:16;
    uint32_t in_link_cnt : 5;          // 入度数量
    uint32_t out_link_cnt : 5;         // 出度数量
    uint32_t available : 1;            // 0:不可通行，1 可通行
    uint32_t direction : 1;            // 0:和形状点序相同，1：和形状点序相反
    uint32_t reserve : 4;
    
};

class TOPOINFO : public Topo {
public:
    TOPOINFO () {
        pre_links = NULL;
        next_links = NULL;
    }
    TOPOINFO (Topo &t) : Topo(t) {
        pre_links = NULL;
        next_links = NULL;
    }

    void setTopo(const Topo *topo) {
        memcpy(this, topo, sizeof(Topo));
    }

    TopoLink *pre_links; // 为数组，指向多个存储的进入拓扑信息
    TopoLink *next_links;

};
//形状信息
struct PointCoor{
	PointCoor(uint32_t x,uint32_t y):point_x(x),point_y(y){}
    uint32_t point_x;
    uint32_t point_y;
};

struct RoadCoor{
public:
    PointCoor* ListCoor(){return (PointCoor*)(this + 1);}
    PointCoor rect_l;
    PointCoor rect_r;
    uint32_t point_num;
};


#endif /* route_struct_h */
