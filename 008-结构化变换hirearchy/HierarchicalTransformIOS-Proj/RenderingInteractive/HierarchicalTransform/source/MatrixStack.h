#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#pragma once

#include "matrix4.h"

#define MAX_MATRIX_NUM 200

typedef struct
{
	unsigned char matrixIndex;

	Matrix4	matrix[MAX_MATRIX_NUM];
	
} MatrixStackData;


class MatrixStack
{
public:

	MatrixStack(void);
	~MatrixStack(void);


	void MatrixStackError( void );

	void MatrixStackPushMatrix( void );

	void MatrixStackPopMatrix( void );

	void MatrixStackLoadMatrix(Matrix4 *m);

	void MatrixStackMultiplyMatrix(Matrix4 *m);

	void TransformTranslate( float x, float y, float z );

	void TransformRotate( float angle, float x, float y, float z );

	void TransformScale( float x, float y, float z );

	Matrix4 *MatrixStackGetTopMatrix( void );
    
	MatrixStackData TransformMemData;
};

#endif //MATRIXSTACK_H
