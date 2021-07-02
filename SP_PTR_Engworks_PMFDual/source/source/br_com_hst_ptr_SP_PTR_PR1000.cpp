#include "br_com_hst_ptr_SP_PTR_PR1000.h"
#include "CEFTMFPTR.h"
#include "CBuffer.h"
#include "CObjectContainer.h"

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1initialize(JNIEnv* env, jobject obj)
{
	CObjectContainer::getTraceInstance()->logAscii("br_com_hst_ptr_SP_1PTR_1PR1000.cpp", "Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1initialize", P_HIGH, "Initialized");
	return (jint) CEF_TMF_PTR_Initialize();
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1finalize(JNIEnv* env, jobject obj)
{
	return (jint) CEF_TMF_PTR_Terminate();
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1reset(JNIEnv* env, jobject obj)
{
	return (jint) CEF_TMF_PTR_Reset();
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1printLine(JNIEnv* env, jobject obj, jbyteArray data, jint dataLength)
{
	jbyte *buffer = env->GetByteArrayElements(data, NULL);

	CBuffer buffData((unsigned char *) buffer, env->GetArrayLength(data));

	return (jint) CEF_TMF_PTR_PrintLine((unsigned char *) buffData.getBuffer(), buffData.length());
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1printDoc(JNIEnv* env, jobject obj, jbyteArray data, jint dataLength)
{
	jbyte *buffer = env->GetByteArrayElements(data, NULL);

	CBuffer buffData((unsigned char *) buffer, env->GetArrayLength(data));

	return (jint) CEF_TMF_PTR_PrintDoc((unsigned char *) buffData.getBuffer(), buffData.length());
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1printRawData(JNIEnv* env, jobject obj, jbyteArray data, jint dataLength)
{
	jbyte *buffer = env->GetByteArrayElements(data, NULL);

	CBuffer buffData((unsigned char *) buffer, env->GetArrayLength(data));

	return (jint)CEF_TMF_PTR_PrintRawData((unsigned char *) buffData.getBuffer(), buffData.length());
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1getStatus(JNIEnv* env, jobject obj)
{
	return (jint) CEF_TMF_PTR_GetStatus();
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1isDocumentInserted(JNIEnv* env, jobject obj)
{
	CEF_TMF_PTR_GetStatus();
	return (jint) CEF_TMF_PTR_DocumentInserted();
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1IOControl(JNIEnv* env, jobject obj, jint, jobject pArg)
{
	return 1;
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1getVersion(JNIEnv* env, jobject obj, jobject pArg)
{
	return 1;
}

JNIEXPORT jint JNICALL Java_br_com_hst_ptr_SP_1PTR_1PR1000_PTR_1PR1000_1receiptCut(JNIEnv* env, jobject obj, jint uiMode)
{
	return (jint) CEF_TMF_PTR_ReceiptCut((int) uiMode);
}


