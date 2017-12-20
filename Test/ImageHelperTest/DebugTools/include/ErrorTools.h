#ifndef _ERROR_TOOLS_H_
#define _ERROR_TOOLS_H_

/*
 * FILE_NUM
 * 每个文件开始都必须定义FILE_NUM,.h文件中不要定义FILE_NUM
 * 定义格式：
 * #undef FILE_NUM
 * #define FILE_NUM 0xCC00
 * CC即为文件编号
 */

/*
 * FUNC_CODE
 * 每个函数都必须定义FUNC_CODE
 * 定义格式：
 * #undef FUNC_CODE
 * #define FUNC_CODE 0xCC
 * CC即为函数编号
 */

/*
 * errCode
 * 即错误码，由4个字节组成，0xTCCC
 * 其中：T为类型
 *           0~7不表示错误
 *           8表示我们的错误, 可用下面ERROR_OWNER
 *           9表示系统错误，可用下面ERROR_SYSTEM
 *           A表示OpenCL错误，可用下面ERROR_OCL
 *           B表示OpenGL错误，可用下面ERROR_OGL
 *        CCC：错误码，应该为正数，如果像OpenCL这样的错误码负数的，应该取绝对值
 * 例如：0x8000 | 0x111, 0xA000 | (-err)， ERROR_OWNER | 0x001, ERROR_OCL | (-err)
 */
#define ERROR_OWNER  0x8000
#define ERROR_SYSTEM 0x9000
#define ERROR_OCL    0xA000
#define ERROR_OGL    0xB000

/*
 * ERROR_CODE: 编码错误码
 */
#define ERROR_CODE(errCode) (((FILE_NUM) & 0x0000FF00) | ((FUNC_CODE) & 0x000000FF) | (((errCode) & 0xFFFF) << 16))

#define ERROR_CHECK(errCode) ((errCode) < 0)

#define ERROR_MESSAGE(errCode, msg) if(ERROR_CHECK(errCode)) LOGE("%s: 0x%x\n", msg, errCode);

#define ERROR_RETURN(errCode, msg) if(ERROR_CHECK(errCode)){LOGE("%s: 0x%x\n", msg, errCode); return errCode;}

#define ERROR_BREAK(errCode, msg) if(ERROR_CHECK(errCode)){LOGE("%s: 0x%x\n", msg, errCode); break;}

#define ERROR_GOTO(errCode, msg, MASK) if(ERROR_CHECK(errCode)){LOGE("%s: 0x%x\n", msg, errCode); goto MASK;}

#ifdef ERROR_CONTINUE
#undef ERROR_CONTINUE
#endif
#define ERROR_CONTINUE(errCode, msg) if(ERROR_CHECK(errCode)){LOGE("%s: 0x%x\n", msg, errCode); continue;}

#define CHECK_OPENCL_ERROR(errCode, msg) if(0 != (errCode)){LOGE("%s: %d\n", msg, errCode); return ERROR_CODE((-(errCode)) | ERROR_OCL);}

#define CHECK_OPENGL_ERROR(errCode, msg) if(0 != (errCode)){LOGE("%s: %d\n", msg, errCode); return ERROR_CODE((-(errCode)) | ERROR_OGL);}

#define CHECK_SYSTEM_IS_ZERO(errCode, msg) if(0 != (errCode)){LOGE("%s: %d\n", msg, errCode); return ERROR_CODE((-(errCode)) | ERROR_SYSTEM);}

#define CHECK_SYSTEM_VALUE_EQUAL(value1, value2, errCode, msg) if((value1) != (value2)){LOGE("%s: %d\n", msg, ERROR_CODE((errCode) | ERROR_SYSTEM)); return ERROR_CODE((errCode) | ERROR_SYSTEM);}

#define CHECK_OWNER_VALUE_EQUAL(value1, value2, errCode, msg) if((value1) != (value2)){LOGE("%s: %d\n", msg, ERROR_CODE((errCode) | ERROR_OWNER)); return ERROR_CODE((errCode) | ERROR_OWNER);}

#define OWN_ERROR_RETURN(errCode, msg) {LOGE("%s: 0x%x\n", msg, ERROR_CODE((errCode) | ERROR_OWNER)); return ERROR_CODE((errCode) | ERROR_OWNER);}

#endif