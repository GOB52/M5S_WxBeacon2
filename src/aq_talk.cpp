/*!
  AquesTalk play with M5.Speaker
  From https://github.com/m5stack/M5Unified/tree/master/examples/Advanced/Speak_with_AquesTalk
 */
#include <M5Unified.hpp>
#include "aq_talk.hpp"
#include <aquestalk.h>
#include <esp_system.h>
#include <cstddef>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include "wb2/wxbeacon2_log.hpp"

namespace
{
// weatcher codeto aqtalk
PROGMEM const char wc_100[] = "hare";
PROGMEM const char wc_101[] = "hare,tokidoki'kumori";
PROGMEM const char wc_102[] = "hare,ichiji'ame";
PROGMEM const char wc_103[] = "hare,tokidoki'ame";
PROGMEM const char wc_104[] = "hare,ichiji'yuki";
PROGMEM const char wc_105[] = "hare'tokidoki'yuki";
PROGMEM const char wc_106[] = "hare,ichiji'ame'kayuki";
PROGMEM const char wc_107[] = "hare,tokidoki'ame'kayuki";
PROGMEM const char wc_108[] = "hare,ichiji'ame'karai'u";
PROGMEM const char wc_110[] = "hare,nochitokidoki'kumori";
PROGMEM const char wc_111[] = "hare,nochi'kumori";
PROGMEM const char wc_112[] = "hare,nochiichiji'ame";
PROGMEM const char wc_113[] = "hare,nochitokidoki'ame";
PROGMEM const char wc_114[] = "hare,nochi'ame";
PROGMEM const char wc_115[] = "hare,nochiichiji'yuki";
PROGMEM const char wc_116[] = "hare,nochitokidoki'yuki";
PROGMEM const char wc_117[] = "hare,nochi'yuki";
PROGMEM const char wc_118[] = "hare,nochi/ame'kayuki";
PROGMEM const char wc_119[] = "hare,nochi/ame'karai'u";
PROGMEM const char wc_120[] = "hare,asayuuichiji'ame";
PROGMEM const char wc_121[] = "hare,asanouchiichiji'ame";
PROGMEM const char wc_122[] = "hare,yuugataichiji'ame";
PROGMEM const char wc_123[] = "hare,yamazoi'rai'u";
PROGMEM const char wc_124[] = "hare,yamazoi'yuki";
PROGMEM const char wc_125[] = "hare,gogowa'rai'u";
PROGMEM const char wc_126[] = "hare,hirugorokara'ame";
PROGMEM const char wc_127[] = "hare,yuugatakara'ame";
PROGMEM const char wc_128[] = "hare,yoruwa'ame";
PROGMEM const char wc_130[] = "asanouchikiri,nochi'hare";
PROGMEM const char wc_131[] = "hare,akegata'kiri";
PROGMEM const char wc_132[] = "hare,asayuu'kumori";
PROGMEM const char wc_140[] = "hare,tokidoki'amede/kaminario'tomonau";
PROGMEM const char wc_160[] = "hare,ichiji'yuki'kaame";
PROGMEM const char wc_170[] = "hare,tokidoki'yuki'kaame";
PROGMEM const char wc_181[] = "hare,nochi'yuki'kaame";

PROGMEM const char wc_200[] = "kumori";
PROGMEM const char wc_201[] = "kumori,tokidoki'hare";
PROGMEM const char wc_202[] = "kumori,ichiji'ame";
PROGMEM const char wc_203[] = "kumori,tokidoki'ame";
PROGMEM const char wc_204[] = "kumori,ichiji'yuki";
PROGMEM const char wc_205[] = "kumori,tokidoki'yuki";
PROGMEM const char wc_206[] = "kumori,ichiji'ame'kayuki";
PROGMEM const char wc_207[] = "kumori,tokidoki'ame'kayuki";
PROGMEM const char wc_208[] = "kumori,ichiji'ame'karai'u";
PROGMEM const char wc_209[] = "kiri";
PROGMEM const char wc_210[] = "kumori,nochitokidoki'hare";
PROGMEM const char wc_211[] = "kumori,nochi'hare";
PROGMEM const char wc_212[] = "kumori,nochiichiji'ame";
PROGMEM const char wc_213[] = "kumori,nochitokidoki'ame";
PROGMEM const char wc_214[] = "kumori,nochi'ame";
PROGMEM const char wc_215[] = "kumori,nochiichiji'yuki";
PROGMEM const char wc_216[] = "kumori,nochitokidoki'yuki";
PROGMEM const char wc_217[] = "kumori,nochi'yuki";
PROGMEM const char wc_218[] = "kumori,nochi/ame'kayuki";
PROGMEM const char wc_219[] = "kumori,nochi/ame'karai'u";
PROGMEM const char wc_220[] = "kumori,asayuuichiji'ame";
PROGMEM const char wc_221[] = "kumori,asanouchiichiji'ame";
PROGMEM const char wc_222[] = "kumori,yuugataichiji'ame";
PROGMEM const char wc_223[] = "kumori,niccyu-tokidoki'hare";
PROGMEM const char wc_224[] = "kumori,hirugorokara'ame";
PROGMEM const char wc_225[] = "kumori,yuugatakara'ame";
PROGMEM const char wc_226[] = "kumori,yoruwa'ame";
PROGMEM const char wc_228[] = "kumori,hirugorokara'yuki";
PROGMEM const char wc_229[] = "kumori,yuugatakara'yuki";
PROGMEM const char wc_230[] = "kumori,yoruwa'yuki";
PROGMEM const char wc_231[] = "kumori,kaijyo-kaiganwa'kiri'ka'kirisame";
PROGMEM const char wc_240[] = "kumori,tokidoki'amede/kaminariotomonau";
PROGMEM const char wc_250[] = "kumori,tokidoki'yukide/kaminariotomonau";
PROGMEM const char wc_260[] = "kumori,ichiji'yuki'kaame";
PROGMEM const char wc_270[] = "kumori,tokidoki'yuki'kaame";
PROGMEM const char wc_281[] = "kumori,nochi'yuki'kaame";

PROGMEM const char wc_300[] = "a'me";
PROGMEM const char wc_301[] = "a'me,tokidoki'hare";
PROGMEM const char wc_302[] = "a'me,tokidoki'yamu";
PROGMEM const char wc_303[] = "a'me,tokidoki'yuki";
PROGMEM const char wc_304[] = "a'me'kayuki";
PROGMEM const char wc_306[] = "ooa'me";
PROGMEM const char wc_308[] = "a'mede,bo-fu-uo'tomonau";
PROGMEM const char wc_309[] = "a'me,ichiji'yuki";
PROGMEM const char wc_311[] = "a'me,nochi'hare";
PROGMEM const char wc_313[] = "a'me,nochi'kumori";
PROGMEM const char wc_314[] = "a'me,nochitokidoki'yuki";
PROGMEM const char wc_315[] = "a'me,nochi'yuki";
PROGMEM const char wc_316[] = "ame/ka'yuki,nochi'hare";
PROGMEM const char wc_317[] = "ame/ka'yuki,nochi'kumori";
PROGMEM const char wc_320[] = "asanouchi,a'me,nochi'hare";
PROGMEM const char wc_321[] = "asanouchi,a'me,nochi'kumori";
PROGMEM const char wc_322[] = "a'me,asabann/ichiji'yuki";
PROGMEM const char wc_323[] = "a'me,hirugorokara'hare";
PROGMEM const char wc_324[] = "a'me,yuugatajarawa'hare";
PROGMEM const char wc_325[] = "a'me,yoruwa'hare";
PROGMEM const char wc_326[] = "a'me,yuugatakara'yuki";
PROGMEM const char wc_327[] = "a'me,yoruwa'yuki";
PROGMEM const char wc_328[] = "a'me,ichiji'tuyoku'furu";
PROGMEM const char wc_329[] = "a'me,ichiji'mizore";
PROGMEM const char wc_340[] = "yuki/ka'ame";
PROGMEM const char wc_350[] = "a'mede,kaminario'tomonau";
PROGMEM const char wc_361[] = "yuki/ka'ame,nochi'hare";
PROGMEM const char wc_371[] = "yuki/ka'ame,nochi'kumori";

PROGMEM const char wc_400[] = "yuki";
PROGMEM const char wc_401[] = "yuki,tokidoki'hare";
PROGMEM const char wc_402[] = "yuki,tokidokiyamu";
PROGMEM const char wc_403[] = "yuki,tokidoki'ame";
PROGMEM const char wc_405[] = "ooyuki";
PROGMEM const char wc_406[] = "fu-setsu,tsuyoi";
PROGMEM const char wc_407[] = "bo-'fu-setsu";
PROGMEM const char wc_409[] = "yuki,ichji'ame";
PROGMEM const char wc_411[] = "yuki,nochi'hare";
PROGMEM const char wc_413[] = "yuki,nochi'kumori";
PROGMEM const char wc_414[] = "yuki,onchi'ame";
PROGMEM const char wc_420[] = "asanouchi,yuki,nochi'hare";
PROGMEM const char wc_421[] = "asanouchi,yuki,nochi'kumori";
PROGMEM const char wc_422[] = "yuki,hirugorokara'ame";
PROGMEM const char wc_423[] = "yuki,yuugatakara'ame";
PROGMEM const char wc_425[] = "yuki,ichiji'tsuyoku'furu";
PROGMEM const char wc_426[] = "yuki,nochi'mizore";
PROGMEM const char wc_427[] = "yuki,ichiji'mizore";
PROGMEM const char wc_450[] = "yukide,kaminario'tomonau";

const std::map<jma::weathercode_t, const char*> weatherCodeTable =
{
    { 100, wc_100 },
    { 101, wc_101 },
    { 102, wc_102 },
    { 103, wc_103 },
    { 104, wc_104 },
    { 105, wc_105 },
    { 106, wc_106 },
    { 107, wc_107 },
    { 108, wc_108 },
    { 110, wc_110 },
    { 111, wc_111 },
    { 112, wc_112 },
    { 113, wc_113 },
    { 114, wc_114 },
    { 115, wc_115 },
    { 116, wc_116 },
    { 117, wc_117 },
    { 118, wc_118 },
    { 119, wc_119 },
    { 120, wc_120 },
    { 121, wc_121 },
    { 122, wc_122 },
    { 123, wc_123 },
    { 124, wc_124 },
    { 125, wc_125 },
    { 126, wc_126 },
    { 127, wc_127 },
    { 128, wc_128 },
    { 130, wc_130 },
    { 131, wc_131 },
    { 132, wc_132 },
    { 140, wc_140 },
    { 160, wc_160 },
    { 170, wc_170 },
    { 181, wc_181 },
    { 200, wc_200 },
    { 201, wc_201 },
    { 202, wc_202 },
    { 203, wc_203 },
    { 204, wc_204 },
    { 205, wc_205 },
    { 206, wc_206 },
    { 207, wc_207 },
    { 208, wc_208 },
    { 209, wc_209 },
    { 210, wc_210 },
    { 211, wc_211 },
    { 212, wc_212 },
    { 213, wc_213 },
    { 214, wc_214 },
    { 215, wc_215 },
    { 216, wc_216 },
    { 217, wc_217 },
    { 218, wc_218 },
    { 219, wc_219 },
    { 220, wc_220 },
    { 221, wc_221 },
    { 222, wc_222 },
    { 223, wc_223 },
    { 224, wc_224 },
    { 225, wc_225 },
    { 226, wc_226 },
    { 228, wc_228 },
    { 229, wc_229 },
    { 230, wc_230 },
    { 231, wc_231 },
    { 240, wc_240 },
    { 250, wc_250 },
    { 260, wc_260 },
    { 270, wc_270 },
    { 281, wc_281 },
    { 300, wc_300 },
    { 301, wc_301 },
    { 302, wc_302 },
    { 303, wc_303 },
    { 304, wc_304 },
    { 306, wc_306 },
    { 308, wc_308 },
    { 309, wc_309 },
    { 311, wc_311 },
    { 313, wc_313 },
    { 314, wc_314 },
    { 315, wc_315 },
    { 316, wc_316 },
    { 317, wc_317 },
    { 320, wc_320 },
    { 321, wc_321 },
    { 322, wc_322 },
    { 323, wc_323 },
    { 324, wc_324 },
    { 325, wc_325 },
    { 326, wc_326 },
    { 327, wc_327 },
    { 328, wc_328 },
    { 329, wc_329 },
    { 340, wc_340 },
    { 350, wc_350 },
    { 361, wc_361 },
    { 371, wc_371 },
    { 400, wc_400 },
    { 401, wc_401 },
    { 402, wc_402 },
    { 403, wc_403 },
    { 405, wc_405 },
    { 406, wc_406 },
    { 407, wc_407 },
    { 409, wc_409 },
    { 411, wc_411 },
    { 413, wc_413 },
    { 414, wc_414 },
    { 420, wc_420 },
    { 421, wc_421 },
    { 422, wc_422 },
    { 423, wc_423 },
    { 425, wc_425 },
    { 426, wc_426 },
    { 427, wc_427 },
    { 450, wc_450 },
};

PROGMEM const char oc_16000[] = "sapporo";
PROGMEM const char oc_40000[] = "sendai";
PROGMEM const char oc_130000[] = "to-kyo-";
PROGMEM const char oc_150000[] = "niigata";
PROGMEM const char oc_170000[] = "kanazawa";
PROGMEM const char oc_230000[] = "nagoya";
PROGMEM const char oc_270000[] = "o-saka";
PROGMEM const char oc_340000[] = "hirosima";
PROGMEM const char oc_390000[] = "ko-chi";
PROGMEM const char oc_400000[] = "fukuoka";
PROGMEM const char oc_471000[] = "naha";

const std::map<jma::officecode_t, const char*> officeCodeTable =
{
    {  16000, oc_16000 },
    {  40000, oc_40000 },
    { 130000, oc_130000 },
    { 150000, oc_150000 },
    { 170000, oc_170000 },
    { 230000, oc_230000 },
    { 270000, oc_270000 },
    { 340000, oc_340000 },
    { 390000, oc_390000 },
    { 400000, oc_400000 },
    { 471000, oc_471000 },
};

constexpr uint8_t m5spk_virtual_channel = 0;
constexpr std::size_t LEN_FRAME = 32;

aq_talk::aq_talk_callback callbackOnEnd;
uint32_t workbuf[AQ_SIZE_WORKBUF];
TaskHandle_t task_handle = nullptr;
volatile bool is_talking = false;
volatile uint32_t level = 0;

std::vector<std::string> koeQueue;
uint16_t koeSpeed = 100;
uint16_t koeLenPause = 256U;

std::vector<std::string> split(const char* s, const char* separator = ",")
{
    std::vector<std::string> out;
    char dup[strlen(s) + 1];
    snprintf(dup, sizeof(dup), "%s", s);

    auto p = strtok(dup, separator);
    if(p)
    {
        out.emplace_back(p);
        while((p = strtok(nullptr, separator))) { out.emplace_back(p); }
    }
    return out;
}

void calcGainAverage(const int16_t* wav, const size_t len)
{
    uint32_t acc = 0;
    for(size_t i=0;i < len; ++i)
    {
        acc += std::abs(*wav++);
    }
    level = acc / len;
}

void talk_task(void*)
{
    int16_t wav[3][LEN_FRAME];
    int tri_index = 0;
    for (;;)
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY ); // wait notify

        while (is_talking)
        {
            uint16_t len = 0;
            if (CAqTkPicoF_SyntheFrame(wav[tri_index], &len))
            {
                if(koeQueue.empty())
                {
                    is_talking = false;
                    break;
                }
                // Connect
                auto ret = CAqTkPicoF_SetKoe(reinterpret_cast<const uint8_t*>(koeQueue.front().c_str()), koeSpeed, koeLenPause);
                WB2_LOGD("Connect Koe %d [%s]", ret, koeQueue.front().c_str());
                koeQueue.erase(koeQueue.begin());
                continue;
            }
            if(len)
            {
                calcGainAverage(wav[tri_index], len);
                M5.Speaker.playRaw(wav[tri_index], len, 8000, false, 1, m5spk_virtual_channel, false);
                tri_index = tri_index < 2 ? tri_index + 1 : 0;
            }
            delay(2);
        }
        if(callbackOnEnd) { callbackOnEnd(); }
    }
}
//
}

namespace aq_talk
{
bool initialize(const UBaseType_t priority, const BaseType_t core, aq_talk_callback f)
{
    if(!task_handle)
    {
        xTaskCreateUniversal(talk_task, "aq_talk_task", 4 * 1024, nullptr, priority, &task_handle, core);
        callbackOnEnd = f;
        return task_handle
                && CAqTkPicoF_Init(workbuf, LEN_FRAME, "XXX-XXX-XXX") == 0; // 
    }
    return true;
}

void finalize()
{
    if(task_handle)
    {
        stopAquesTalk();
        vTaskDelete(task_handle);
        task_handle = 0;
    }
}

uint32_t getGain() { return is_talking ? level : 0; }

bool busy() { return is_talking; }

void waitAquesTalk(void)
{
    while (is_talking) { vTaskDelay(1); }
    koeQueue.clear();
}

void stopAquesTalk(void)
{
    if (is_talking) { is_talking = false; vTaskDelay(1); }
}

bool playAquesTalk(const char *koe, const uint16_t speed, const uint16_t lenPause)
{
    stopAquesTalk();
    waitAquesTalk();

    
    // Split and store queue. (CAqTkPicoF_SetKoe has a length limitation)
    koeQueue.clear();
    koeQueue = split(koe, "#");
    koeSpeed = speed;
    koeLenPause = lenPause;
    WB2_LOGV("queue:%zu", koeQueue.size());
    
    if(koeQueue.empty()) { return false; }

    //    auto ret = CAqTkPicoF_SetKoe(reinterpret_cast<const uint8_t*>(koe), speed, lenPause);
    auto ret = CAqTkPicoF_SetKoe(reinterpret_cast<const uint8_t*>(koeQueue.front().c_str()), speed, lenPause);
    WB2_LOGD("Koe %d [%s]", ret, koeQueue.front().c_str());
    koeQueue.erase(koeQueue.begin());
    if(ret == 0)
    {
        is_talking = true;
        xTaskNotifyGive(task_handle);
    }
    return ret == 0;
}

const char* weatherCodeToTalk(const jma::weathercode_t wc)
{
    return weatherCodeTable.count(wc) == 1 ? weatherCodeTable.at(wc) : "";
}

const char* officeCodeToTalk(const jma::officecode_t oc)
{
    return officeCodeTable.count(oc) == 1 ? officeCodeTable.at(oc) : "";
}

//
}
