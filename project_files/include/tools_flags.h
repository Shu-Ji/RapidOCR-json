#ifndef _tools_flags_
#define _tools_flags_

// �����ڲ�ʹ�ã�������ı�־��
#define CODE_INIT                0   // ÿ�غϳ�ʼֵ���غϽ���ʱ��Ϊ�������ܹܿص�������δ���ִ���
#define CODE_ERR_MAT_NULL       -999 // OCR������matΪ�ա����� dbNetTime �����ݡ�

// ����ı�־��

// ʶ��ɹ�
#define CODE_OK                 100 // �ɹ�����ʶ�������
#define CODE_OK_NONE            101 // �ɹ�����δʶ������
#define MSG_OK_NONE(p)          "No text found in image. Path: \""+p+"\""
// ��·����ͼ��ʧ��
#define CODE_ERR_PATH_EXIST     200 // ͼƬ·��������
#define MSG_ERR_PATH_EXIST(p)   "Image path dose not exist. Path: \""+p+"\""
#define CODE_ERR_PATH_CONV      201 // ͼƬ·��string�޷�ת����wstring
#define MSG_ERR_PATH_CONV(p)    "Image path failed to convert to utf-16 wstring. Path: \""+p+"\""
#define CODE_ERR_PATH_READ      202 // ͼƬ·�����ڣ����޷����ļ�
#define MSG_ERR_PATH_READ(p)    "Image open failed. Path: \""+p+"\""
#define CODE_ERR_PATH_DECODE    203 // ͼƬ�򿪳ɹ�������ȡ���������޷���opencv����
#define MSG_ERR_PATH_DECODE(p)  "Image decode failed. Path: \""+p+"\""
#define CODE_ERR_LOAD_JSON      204 // ����JSONʧ��
#define MSG_ERR_LOAD_JSON(p)    "Failed to load the string into JSON. Input: \""+p+"\""
#define CODE_ERR_JSON_NO_IMAGE   205 // json�в�����ͼƬ
#define MSG_ERR_JSON_NO_IMAGE(p) "No 'imagePath' element in the JSON string. Input: \""+p+"\""
// �������ͼ��ʧ��
#define CODE_ERR_CLIP_OPEN      210 // �������ʧ�� ( OpenClipboard )
#define MSG_ERR_CLIP_OPEN       "Clipboard open failed."
#define CODE_ERR_CLIP_EMPTY     211 // ������Ϊ�� ( GetPriorityClipboardFormat NULL )
#define MSG_ERR_CLIP_EMPTY      "Clipboard is empty."
#define CODE_ERR_CLIP_FORMAT    212 // ������ĸ�ʽ��֧�� ( GetPriorityClipboardFormat -1 )
#define MSG_ERR_CLIP_FORMAT     "Clipboard format is not valid."
#define CODE_ERR_CLIP_DATA      213 // �������ȡ���ݾ��ʧ�ܣ�ͨ���ɱ�ĳ���ռ�ü��������� ( GetClipboardData NULL )
#define MSG_ERR_CLIP_DATA       "Getting clipboard data handle failed."
#define CODE_ERR_CLIP_FILES     214 // �������ѯ�����ļ���������Ϊ1 ( DragQueryFile != 1 )
#define MSG_ERR_CLIP_FILES(n)   "Clipboard number of query files is not valid. Number: "+std::to_string(n)
#define CODE_ERR_CLIP_GETOBJ    215 // ���������ͼ�ζ�����Ϣʧ�� ( GetObject NULL )
#define MSG_ERR_CLIP_GETOBJ     "Clipboard get bitmap object failed."
#define CODE_ERR_CLIP_BITMAP    216 // �������ȡλͼ����ʧ�� ( GetBitmapBits �����ֽ�Ϊ�� )
#define MSG_ERR_CLIP_BITMAP     "Getting clipboard bitmap bits failed."
#define CODE_ERR_CLIP_CHANNEL   217 // ��������λͼ��ͨ������֧�� ( nChannels ��Ϊ1��3��4 )
#define MSG_ERR_CLIP_CHANNEL(n) "Clipboard number of image channels is not valid. Number: "+std::to_string(n)
// δ֪
#define CODE_ERR_UNKNOW        299 // δ֪�쳣
#define MSG_ERR_UNKNOW         "An unknown error has occurred."


#endif