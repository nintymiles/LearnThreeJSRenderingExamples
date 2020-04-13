//
//  RayCaster.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef RayCaster_h
#define RayCaster_h

#include <stdio.h>

#include "cvec.h"
#include "matrix4.h"
#include "Ray.h"
#include "Camera.h"
#include "CubeModel.h"

//class CubeModel;
class RayCaster{

public:
    Ray* ray;
    float near;
    float far;
    
    RayCaster(){
        ray=new Ray();
    }
    ~RayCaster(){};
    
    //借助相机数据生成world frame中的screen-ray
    //屏幕点先是被对应为near plane上的点，然后再转换为world coordinate
    //也就是说先进行反转viewport计算，然后在应用unporjection矩阵，最后应用eye matrix（inverse view matrix）
    void setFromCamera(Cvec3 screenPos,shared_ptr<PerspectiveCamera> camera){
        //反转viewport计算，由于窗口坐标y轴的原点在窗口上方，所以需要反转符号
        float rayOriginX = (screenPos[0]/camera->view.width) * 2 - 1;
        float rayOriginY = -(screenPos[1]/camera->view.height) * 2 + 1;
        
        if(camera){
            Matrix4 eyeMat = camera->eyeMat;
            Matrix4 projMat = camera->projMat;
            Cvec3 camPosition = vec3(eyeMat(0,3),eyeMat(1,3),eyeMat(2,3));
            Cvec4 screenPosWorld= (eyeMat*inv(projMat)) * vec4(rayOriginX,rayOriginY,1.0f,1.0f);
            //反转投射矩阵应用后的坐标仍为同质坐标，需要执行除法以获得放射坐标
            screenPosWorld = screenPosWorld/screenPosWorld[3];
            //screen ray方向矢量需要标准化
            Cvec3 rayDi =normalize(vec3(screenPosWorld) -camPosition);
            
            ray = new Ray(camPosition,rayDi);
        }
        
    }
    
    vector<IntersectionData> intersectObject(CubeModel* model){
        if(model->isPickingEnabled){
            return model->rayCast(this);
        }
    
        return vector<IntersectionData>();
    }
    
    vector<IntersectionData> intersectObjects(vector<shared_ptr<CubeModel>> objects){
        vector<IntersectionData> intersects;
        for(auto object:objects){
            if(object->isPickingEnabled){
                vector<IntersectionData> intersectData=object->rayCast(this);
                for(auto data:intersectData)
                    intersects.push_back(data);
            }
        }
        return intersects;
    }
};

#endif /* RayCaster_h */
