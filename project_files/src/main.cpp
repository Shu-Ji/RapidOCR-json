// ������� https://github.com/RapidAI/RapidOcrOnnx
// ���ο��� by https://github.com/hiroi-sora


// �汾��Ϣ
#define PROJECT_VER "v1.0.0"
#define PROJECT_NAME "RapidOCR-json " PROJECT_VER

#ifndef __JNI__
#ifndef __CLIB__
#include <cstdio>
#include <iostream>
#include <ctime> // ��¼ʱ����� 
#include "main.h"
#include "version.h"
#include "OcrLite.h"
#include "OcrUtils.h"
#include "nlohmann_json.hpp"
#include "tools.h"
#include "tools_flags.h" // ��־
#ifdef _WIN32
#include <windows.h>
#endif
using namespace tool;

OcrLite *OCRLiteP = NULL; // ����ָ�� 

void printHelp(FILE *out, char *argv0) {
    fprintf(out, " ------- Usage -------\n");
    fprintf(out, "%s\n\n", argv0);
    fprintf(out, " ------- Optional Parameters -------\n");
    fprintf(out, "%s", optionalMsg);
    fprintf(out, " ------- Other Parameters -------\n");
    fprintf(out, "%s", otherMsg);
    fprintf(out, " ------- Examples -------\n");
    fprintf(out, example1Msg, argv0);
    fprintf(out, example2Msg, argv0);
}

// ==================== ִ��һ��OCR���� ==================== 
void runOCR(std::string imgPath) 
{
    OcrLite &ocrLite = *OCRLiteP; // ��ȡ������� 
    std::string imgDir, imgName;
    // ͼƬ·�����
	imgDir.assign(imgPath.substr(0, imgPath.find_last_of('/') + 1));
	imgName.assign(imgPath.substr(imgPath.find_last_of('/') + 1));
    // ��ʼ�����ӻ����
    if (ensureLogger) {
        ocrLite.enableResultTxt(imgDir.c_str(), imgName.c_str());
    }

    // ִ��һ��OCR 
    OcrResult result = ocrLite.detect(imgDir.c_str(), imgName.c_str(), padding, maxSideLen,
        boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    ocrLite.Logger("%s\n", result.strRes.c_str());

    // ���
    // 3.1. �����ʶ��ʧ��
    if (result.dbNetTime == CODE_ERR_MAT_NULL) { // �����ǩ
        tool::print_now_fail(); // �������
        return;
    }
    // 3.2. ��������
    nlohmann::json outJ;
    outJ["code"] = 100;
    outJ["data"] = nlohmann::json::array();
    bool isEmpty = true; // ��¼�Ƿ�Ϊ�ս��
    for (const auto& block : result.textBlocks) {
        if (block.text.empty()) // �Ϸ��Լ��
            continue;
        nlohmann::json j;
        // ��¼����ַ���
        j["text"] = block.text; 
        // ��¼������Ŷ�
        double score = 0; // ƽ���ַ����Ŷ�
        float charScoreSum = 0.0f;
        for (const auto& s : block.charScores) {
            score += s;
        }
        score /= static_cast<double>(block.charScores.size());
        j["score"] = score;
        // ��¼��Χ��
        std::vector<cv::Point> b = block.boxPoint;
        j["box"] = { {b[0].x, b[0].y}, {b[1].x, b[1].y},
            {b[2].x, b[2].y }, { b[3].x, b[3].y } };
        outJ["data"].push_back(j);
        isEmpty = false;
    }
    // 3.3. �����ʶ��ɹ��������֣�recδ�����
    if (isEmpty) {
        print_ocr_fail(CODE_OK_NONE, MSG_OK_NONE(imgPath));
        return;
    }
    // 3.4. ���������������
    else {
        print_json(outJ);
    }

}

// ==================== ����OCRѭ�� ==================== 
void startOCR(std::string imgPath="")
{
    if (!imgPath.empty()) { // ִֻ��һ�� 
        runOCR(imgPath);
    }
    else // �ظ�ִ�� 
    {
        std::string jsonIn;
        while (1) {
            jsonIn = "";
            getline(std::cin, jsonIn);
            int strLen = jsonIn.length();
            // ��Ϊjson�ַ���������� 
            if (strLen > 2 && jsonIn[0] == '{' && jsonIn[strLen - 1] == '}') {
                tool::load_json_str(jsonIn);
                if (jsonIn.empty()) { // δ������ͼƬ
                    tool::print_now_fail(); // �������
                    continue;
                }
            } // ����һ����img_pathΪ utf-8 ��·���ַ���
            runOCR(jsonIn);
        }
    }
}

int main(int argc, char **argv) {
    // ==================== ����ͽ������� ==================== 
    //if (argc <= 1) {
    //    printHelp(stderr, argv[0]);
    //    return -1;
    //}
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::string imgPath;
    int opt;
    int optionIndex = 0;
    // ==================== ���������в��� ==================== 
    // ���ö�ѡ�ԭ��"d:1:2:3:4:i:t:p:s:b:o:u:a:A:G:v:h"
    while ((opt = getopt_long(argc, argv, "", long_options, &optionIndex)) != -1) {
        switch (opt) {
            case 'd': // ģ��Ŀ¼��ַ
                modelsDir = optarg;
                break;
            case '1': // detģ���ļ���
                modelDetPath = optarg;
                break;
            case '2': // clsģ���ļ��� 
                modelClsPath = optarg;
                break;
            case '3': // recģ���ļ��� 
                modelRecPath = optarg;
                break;
            case '4': // key�ֵ��ļ��� 
                keysPath = optarg;
                break;
            case 'i': // ��ʼͼƬ·�� 
                imgPath.assign(optarg);
                //imgDir.assign(imgPath.substr(0, imgPath.find_last_of('/') + 1));
                //imgName.assign(imgPath.substr(imgPath.find_last_of('/') + 1));
                break;
            case 't': // �߳��� 
                numThread = (int) strtol(optarg, NULL, 10);
                break;
            case 'p': // ��ͼƬ������ӰױߵĿ�� 
                padding = (int) strtol(optarg, NULL, 10);
                break;
            case 's': // ͼƬ������С 
                maxSideLen = (int) strtol(optarg, NULL, 10);
                break;
            case 'b': // ���ֿ����Ŷ����� 
                boxScoreThresh = strtof(optarg, NULL);
                break;
            case 'o':
                boxThresh = strtof(optarg, NULL);
                break;
            case 'u': // �������ֿ��С���� 
                unClipRatio = strtof(optarg, NULL);
                break;
            case 'a': // ���ַ����� 
                doAngle = (int)strtol(optarg, NULL, 10)==0 ? false : true;
                break;
            case 'A': // �Ƕ�ͶƱ 
                mostAngle = (int)strtol(optarg, NULL, 10)==0 ? false : true;
                break;
            case 'v':
                return 0;
            case 'h': // ���� 
                printHelp(stdout, argv[0]);
                return 0;
            case 'G': // ʹ�õ�GPU��� 
                flagGpu = (int)strtol(optarg, NULL, 10);
                break;
            case 'L': // ��־�Ϳ��ӻ���� 
                ensureLogger = (int)strtol(optarg, NULL, 10) == 0 ? false : true;
                break;
            case 'S': // ASCIIת��  
                ensureAscii = (int)strtol(optarg, NULL, 10) == 0 ? false : true;
                break;
            default:
                printf("other option %c :%s\n", opt, optarg);
        }
    }
    
    // ==================== ���ģ�ʹ��� ==================== 

    modelDetPath = modelsDir + "/" + modelDetPath;
    modelClsPath = modelsDir + "/" + modelClsPath;
    modelRecPath = modelsDir + "/" + modelRecPath;
    keysPath     = modelsDir + "/" + keysPath;
    bool hasModelDetFile = isFileExists(modelDetPath);
    if (!hasModelDetFile) {
        fprintf(stderr, "Model det file not found: %s\n", modelDetPath.c_str());
        return -1;
    }
    bool hasModelClsFile = isFileExists(modelClsPath);
    if (!hasModelClsFile) {
        fprintf(stderr, "Model cls file not found: %s\n", modelClsPath.c_str());
        return -1;
    }
    bool hasModelRecFile = isFileExists(modelRecPath);
    if (!hasModelRecFile) {
        fprintf(stderr, "Model rec file not found: %s\n", modelRecPath.c_str());
        return -1;
    }
    bool hasKeysFile = isFileExists(keysPath);
    if (!hasKeysFile) {
        fprintf(stderr, "keys file not found: %s\n", keysPath.c_str());
        return -1;
    }

    // ==================== ��ʼ��OCR ==================== 
    OcrLite ocrLite;
    ocrLite.initLogger( // ������־���
        ensureLogger,//isOutputConsole
        false,//isOutputPartImg
        ensureLogger);//isOutputResultImg
    ocrLite.setNumThread(numThread); // �����߳� 
    ocrLite.setGpuIndex(flagGpu); // ��������GPU 
    ocrLite.initModels(modelDetPath, modelClsPath, modelRecPath, keysPath); // ����ģ�� 
    ocrLite.Logger("=====Input Params=====\n");
    ocrLite.Logger(
            "numThread(%d),padding(%d),maxSideLen(%d),boxScoreThresh(%f),boxThresh(%f),unClipRatio(%f),doAngle(%d),mostAngle(%d),GPU(%d)\n",
            numThread, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle,
            flagGpu);
    OCRLiteP = &ocrLite;

    std::cout << PROJECT_NAME << std::endl; // �汾��ʾ
    std::cout << "OCR init completed." << std::endl; // �����ʾ

    // ==================== ����OCR ==================== 
    startOCR(imgPath);
    return 0;
}

#endif
#endif