#ifndef __MAIN_H__
#define __MAIN_H__

#include "getopt.h"

// �����б� 
static const struct option long_options[] = {
         // �����������ǰ�趨 
        {"ensureAscii",    required_argument, NULL, 'S'}, // ����(1)/����(0) ASCIIת����� 
        {"ensureLogger",   required_argument, NULL, 'L'}, // ����(1)/����(0) ��־�Ϳ��ӻ���� 
        {"models",         required_argument, NULL, 'd'}, // ģ��Ŀ¼��ַ���ɾ���or���·�� 
        {"det",            required_argument, NULL, '1'}, // detģ���ļ��� 
        {"cls",            required_argument, NULL, '2'}, // clsģ���ļ��� 
        {"rec",            required_argument, NULL, '3'}, // recģ���ļ��� 
        {"keys",           required_argument, NULL, '4'}, // key�ֵ��ļ��� 
        {"numThread",      required_argument, NULL, 't'}, // �߳��� 
        {"GPU",            required_argument, NULL, 'G'}, // ʹ�õ�GPU��ţ���ʹ��Ϊ-1 
        // �Ȳ�������������޸� 
        {"imagePath",      required_argument, NULL, 'i'}, // ��ʼͼƬ·�� 
        {"padding",        required_argument, NULL, 'p'}, // ��ͼƬ������ӰױߵĿ�� 
        {"maxSideLen",     required_argument, NULL, 's'}, // ͼƬ������С 
        {"boxScoreThresh", required_argument, NULL, 'b'}, // ���ֿ����Ŷ����� 
        {"boxThresh",      required_argument, NULL, 'o'},
        {"unClipRatio",    required_argument, NULL, 'u'}, // �������ֿ��С���ʣ�Խ��ʱ�������ֿ�Խ�󡣴�����ͼƬ�Ĵ�С��أ�Խ���ͼƬ��ֵӦ��Խ��
        {"doAngle",        required_argument, NULL, 'a'}, // ����(1)/����(0) ���ַ����� 
        {"mostAngle",      required_argument, NULL, 'A'}, // ����(1)/����(0) �Ƕ�ͶƱ(����ͼƬ�����������ַ�����ʶ��)�����������ַ�����ʱ������Ҳ�������á�
        {"version",        no_argument,       NULL, 'v'},
        {"help",           no_argument,       NULL, 'h'}, // ��ӡ���� 
        {"loopCount",      required_argument, NULL, 'l'},
        {NULL,             no_argument,       NULL, 0}
};


const char *optionalMsg = "--models: models directory.\n" \
                          "--det: model file name of det.\n" \
                          "--cls: model file name of cls.\n" \
                          "--rec: model file name of rec.\n" \
                          "--keys: keys file name.\n" \
                          "--imagePath: If filled, use this path for a single OCR.If left blank, enter OCR work loop.\n"\
                          "--ensureAscii: Enable(1)/Disable(0) output character escaping according to ASCII code.\n"\
                          "--ensureLogger: Enable(1)/Disable(0) logging output and visualization output of image results..\n"\
                          "--numThread: value of numThread(int), default: 4\n" \
                          "--padding: value of padding(int), default: 50\n" \
                          "--maxSideLen: Long side of picture for resize(int), default: 1024\n" \
                          "--boxScoreThresh: value of boxScoreThresh(float), default: 0.5\n" \
                          "--boxThresh: value of boxThresh(float), default: 0.3\n" \
                          "--unClipRatio: value of unClipRatio(float), default: 1.6\n" \
                          "--doAngle: Enable(1)/Disable(0) Angle Net, default: Enable\n" \
                          "--mostAngle: Enable(1)/Disable(0) Most Possible AngleIndex, default: Enable\n" \
                          "--GPU: Disable(-1)/GPU0(0)/GPU1(1)/... Use Vulkan GPU accelerate, default: Disable(-1)\n\n";

const char *otherMsg = "--version: show version\n" \
                       "--help: print this help\n\n";

const char *example1Msg = "Example1: %s --imagePath=\"D:/images/test(1).png\"\n\n";
const char *example2Msg = "Example2: %s --models=models --det=ch_PP-OCRv3_det_infer.onnx --cls=ch_ppocr_mobile_v2.0_cls_infer.onnx --rec=ch_PP-OCRv3_rec_infer.onnx --keys=ppocr_keys_v1.txt --ensureAscii=1 --ensureLogger=1 --numThread=8 --padding=50 --maxSideLen=1024 --boxScoreThresh=0.5 --boxThresh=0.3 --unClipRatio=1.6 --doAngle=1 --mostAngle=1 --GPU=-1\n\n";

#endif //__MAIN_H__
