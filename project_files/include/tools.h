#ifndef _tools_
#define _tools_
#include "tools_flags.h" // ��־
#include "nlohmann_json.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"

namespace tool {
    // ==================== ���� ====================
    // �����������ǰ�趨 
    extern bool ensureAscii; // ���json�Ƿ�תascii 
    extern bool ensureLogger; // ������־��� 
    extern std::string modelsDir; // ģ�Ϳ�Ŀ¼ 
    extern std::string modelDetPath; // Ĭ�ϼ��ģ�� 
    extern std::string modelClsPath; // Ĭ�Ϸ������ģ�� 
    extern std::string modelRecPath; // Ĭ��ʶ��ģ�� 
    extern std::string keysPath; // Ĭ���ֵ� 
    extern int numThread; // �߳��� 
    extern int flagGpu; // ʹ�õ�GPU��ţ���ʹ��Ϊ-1 
    // �Ȳ�������������޸� 
    extern int padding; // Ԥ����ױ߿�� 
    extern int maxSideLen; // ������������ֵ 
    extern float boxScoreThresh; // ���ֿ����Ŷ����� 
    extern float boxThresh;
    extern float unClipRatio; // �������ֿ��С���ʣ�Խ��ʱ�������ֿ�Խ��
    extern bool doAngle; // �������ַ����� 
    extern bool mostAngle; // ���ýǶ�ͶƱ
    extern bool isEnsureAsci; // �Ƿ�תascii 

    // ==================== ���� ====================
    void get_state(int&, std::string&);
    void set_state(int code = CODE_INIT, std::string msg = "");

    void load_congif_file();

	void print_json(const nlohmann::json&);
	void print_ocr_fail(int, const std::string&); 
	void print_now_fail();
	void load_json_str(std::string&);

	cv::Mat imread_utf8(std::string, int flags = cv::IMREAD_COLOR);
}

#endif