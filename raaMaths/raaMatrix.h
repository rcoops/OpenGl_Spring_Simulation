#pragma once
#ifdef _DEBUG
#pragma comment(lib,"raaMathsD")
#else
#pragma comment(lib,"raaMathsR")
#endif


void matMakeIdentity(float *pM);
void matMakeTranslation(float fX, float fY, float fZ, float *pM);
void matMakeScale(float fX, float fY, float fZ, float *pM);
void matMakeXAxisRot(float fRadians, float *pM);
void matMakeYAxisRot(float fRadians, float *pM);
void matMakeZAxisRot(float fRadians, float *pM);
void matTranslate(float fX, float fY, float fZ, float *pM);
void matScale(float fX, float fY, float fZ, float *pM);
void matXAxisRot(float fRadians, float *pM);
void matYAxisRot(float fRadians, float *pM);
void matZAxisRot(float fRadians, float *pM);
void matVecMult(float *pV, float *pM, float *pVRes);
void matMatMult(float *pM0, float *pM1, float *pMRes);
void matInvert(float *pMIn, float *pMOut);
bool matFullInvert(float* pMIn, float* pMOut);
