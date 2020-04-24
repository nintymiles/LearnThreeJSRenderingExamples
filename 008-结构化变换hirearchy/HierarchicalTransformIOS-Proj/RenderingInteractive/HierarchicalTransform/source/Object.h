#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <typeinfo>

class Object{
public:
    Object(std::string name = "", Object* parentObj = NULL);
    virtual ~Object();
    
    void SetName(std::string mdlName){ name = mdlName;}
    std::string GetName(){return name;}
    
    void SetParent(Object* parent = 0);
    void RemoveParent();
    
    void SetChild(Object* child = 0);
    void RemoveFromParentChildList();
    
    Object*  GetParent();
    std::vector<Object*>* GetChildren();
    
protected:
    
    //模型名称
    std::string name;
    
    //父节点
    Object* parent;
    
    //子节点
    std::vector<Object*> childList;
};

#endif // OBJECT_H
