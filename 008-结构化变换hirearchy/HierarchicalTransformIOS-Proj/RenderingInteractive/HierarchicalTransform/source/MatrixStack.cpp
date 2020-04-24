#include "MatrixStack.h"
#include <cstdlib>
#include <string>


MatrixStack::MatrixStack(void)
{
	TransformMemData.matrixIndex = 0;
}

MatrixStack::~MatrixStack(void){
}

//获得当前MatrixIndex所指向的缓存中的matrix的指针
Matrix4* MatrixStack::MatrixStackGetTopMatrix( void )
{
	return &TransformMemData.matrix[TransformMemData.matrixIndex];
}



//实现对不同类型的matrix stack进行操作，维护每种matrix stack的matrix index。
void MatrixStack::MatrixStackPushMatrix( void )
{

    //对matrix缓存索引外溢的检测
    if(TransformMemData.matrixIndex >= MAX_MATRIX_NUM){
        return;
    }


    memcpy( &TransformMemData.matrix[TransformMemData.matrixIndex + 1],
           &TransformMemData.matrix[TransformMemData.matrixIndex],
           sizeof(Matrix4));

    //压栈后将矩阵index增加1
    ++TransformMemData.matrixIndex;
            
			
}


void MatrixStack::MatrixStackPopMatrix( void )
{

    if (TransformMemData.matrixIndex == 0){
        return;
    }
    --TransformMemData.matrixIndex;

}

void MatrixStack::MatrixStackLoadMatrix(Matrix4 *m)
{
    //将参数matrix中的内存值copy到栈顶部矩阵内存中
    memcpy(MatrixStackGetTopMatrix(), m, sizeof(Matrix4));
}

//矩阵乘法的执行将当前栈顶部的矩阵累加当前参数中的矩阵变换，矩阵乘法的语意
void MatrixStack::MatrixStackMultiplyMatrix(Matrix4 *m){
    *MatrixStackGetTopMatrix() *=  (*m);
}
