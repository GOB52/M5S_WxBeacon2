
#include "jma_definition.hpp"
#include <pgmspace.h>
#include <map>
#include <vector>

namespace
{

// Weatcher code string
#if 1

PROGMEM const char wc_100[] = "晴";
PROGMEM const char wc_101[] = "晴時々曇";
PROGMEM const char wc_102[] = "晴一時雨";
PROGMEM const char wc_103[] = "晴時々雨";
PROGMEM const char wc_104[] = "晴一時雪";
PROGMEM const char wc_105[] = "晴時々雪";
PROGMEM const char wc_106[] = "晴一時雨か雪";
PROGMEM const char wc_107[] = "晴時々雨か雪";
PROGMEM const char wc_108[] = "晴一時雨か雷雨";
PROGMEM const char wc_110[] = "晴後時々曇";
PROGMEM const char wc_111[] = "晴後曇";
PROGMEM const char wc_112[] = "晴後一時雨";
PROGMEM const char wc_113[] = "晴後時々雨";
PROGMEM const char wc_114[] = "晴後雨";
PROGMEM const char wc_115[] = "晴後一時雪";
PROGMEM const char wc_116[] = "晴後時々雪";
PROGMEM const char wc_117[] = "晴後雪";
PROGMEM const char wc_118[] = "晴後雨か雪";
PROGMEM const char wc_119[] = "晴後雨か雷雨";
PROGMEM const char wc_120[] = "晴朝夕一時雨";
PROGMEM const char wc_121[] = "晴朝の内一時雨";
PROGMEM const char wc_122[] = "晴夕方一時雨";
PROGMEM const char wc_123[] = "晴山沿い雷雨";
PROGMEM const char wc_124[] = "晴山沿い雪";
PROGMEM const char wc_125[] = "晴午後は雷雨";
PROGMEM const char wc_126[] = "晴昼頃から雨";
PROGMEM const char wc_127[] = "晴夕方から雨";
PROGMEM const char wc_128[] = "晴夜は雨";
PROGMEM const char wc_130[] = "朝の内霧後晴";
PROGMEM const char wc_131[] = "晴明け方霧";
PROGMEM const char wc_132[] = "晴朝夕曇";
PROGMEM const char wc_140[] = "晴時々雨で雷を伴う";
PROGMEM const char wc_160[] = "晴一時雪か雨";
PROGMEM const char wc_170[] = "晴時々雪か雨";
PROGMEM const char wc_181[] = "晴後雪か雨";
PROGMEM const char wc_200[] = "曇";
PROGMEM const char wc_201[] = "曇時々晴";
PROGMEM const char wc_202[] = "曇一時雨";
PROGMEM const char wc_203[] = "曇時々雨";
PROGMEM const char wc_204[] = "曇一時雪";
PROGMEM const char wc_205[] = "曇時々雪";
PROGMEM const char wc_206[] = "曇一時雨か雪";
PROGMEM const char wc_207[] = "曇時々雨か雪";
PROGMEM const char wc_208[] = "曇一時雨か雷雨";
PROGMEM const char wc_209[] = "霧";
PROGMEM const char wc_210[] = "曇後時々晴";
PROGMEM const char wc_211[] = "曇後晴";
PROGMEM const char wc_212[] = "曇後一時雨";
PROGMEM const char wc_213[] = "曇後時々雨";
PROGMEM const char wc_214[] = "曇後雨";
PROGMEM const char wc_215[] = "曇後一時雪";
PROGMEM const char wc_216[] = "曇後時々雪";
PROGMEM const char wc_217[] = "曇後雪";
PROGMEM const char wc_218[] = "曇後雨か雪";
PROGMEM const char wc_219[] = "曇後雨か雷雨";
PROGMEM const char wc_220[] = "曇朝夕一時雨";
PROGMEM const char wc_221[] = "曇朝の内一時雨";
PROGMEM const char wc_222[] = "曇夕方一時雨";
PROGMEM const char wc_223[] = "曇日中時々晴";
PROGMEM const char wc_224[] = "曇昼頃から雨";
PROGMEM const char wc_225[] = "曇夕方から雨";
PROGMEM const char wc_226[] = "曇夜は雨";
PROGMEM const char wc_228[] = "曇昼頃から雪";
PROGMEM const char wc_229[] = "曇夕方から雪";
PROGMEM const char wc_230[] = "曇夜は雪";
PROGMEM const char wc_231[] = "曇海上海岸は霧か霧雨";
PROGMEM const char wc_240[] = "曇時々雨で雷を伴う";
PROGMEM const char wc_250[] = "曇時々雪で雷を伴う";
PROGMEM const char wc_260[] = "曇一時雪か雨";
PROGMEM const char wc_270[] = "曇時々雪か雨";
PROGMEM const char wc_281[] = "曇後雪か雨";
PROGMEM const char wc_300[] = "雨";
PROGMEM const char wc_301[] = "雨時々晴";
PROGMEM const char wc_302[] = "雨時々止む";
PROGMEM const char wc_303[] = "雨時々雪";
PROGMEM const char wc_304[] = "雨か雪";
PROGMEM const char wc_306[] = "大雨";
PROGMEM const char wc_308[] = "雨で暴風を伴う";
PROGMEM const char wc_309[] = "雨一時雪";
PROGMEM const char wc_311[] = "雨後晴";
PROGMEM const char wc_313[] = "雨後曇";
PROGMEM const char wc_314[] = "雨後時々雪";
PROGMEM const char wc_315[] = "雨後雪";
PROGMEM const char wc_316[] = "雨か雪後晴";
PROGMEM const char wc_317[] = "雨か雪後曇";
PROGMEM const char wc_320[] = "朝の内雨後晴";
PROGMEM const char wc_321[] = "朝の内雨後曇";
PROGMEM const char wc_322[] = "雨朝晩一時雪";
PROGMEM const char wc_323[] = "雨昼頃から晴";
PROGMEM const char wc_324[] = "雨夕方から晴";
PROGMEM const char wc_325[] = "雨夜は晴";
PROGMEM const char wc_326[] = "雨夕方から雪";
PROGMEM const char wc_327[] = "雨夜は雪";
PROGMEM const char wc_328[] = "雨一時強く降る";
PROGMEM const char wc_329[] = "雨一時みぞれ";
PROGMEM const char wc_340[] = "雪か雨";
PROGMEM const char wc_350[] = "雨で雷を伴う";
PROGMEM const char wc_361[] = "雪か雨後晴";
PROGMEM const char wc_371[] = "雪か雨後曇";
PROGMEM const char wc_400[] = "雪";
PROGMEM const char wc_401[] = "雪時々晴";
PROGMEM const char wc_402[] = "雪時々止む";
PROGMEM const char wc_403[] = "雪時々雨";
PROGMEM const char wc_405[] = "大雪";
PROGMEM const char wc_406[] = "風雪強い";
PROGMEM const char wc_407[] = "暴風雪";
PROGMEM const char wc_409[] = "雪一時雨";
PROGMEM const char wc_411[] = "雪後晴";
PROGMEM const char wc_413[] = "雪後曇";
PROGMEM const char wc_414[] = "雪後雨";
PROGMEM const char wc_420[] = "朝の内雪後晴";
PROGMEM const char wc_421[] = "朝の内雪後曇";
PROGMEM const char wc_422[] = "雪昼頃から雨";
PROGMEM const char wc_423[] = "雪夕方から雨";
PROGMEM const char wc_425[] = "雪一時強く降る";
PROGMEM const char wc_426[] = "雪後みぞれ";
PROGMEM const char wc_427[] = "雪一時みぞれ";
PROGMEM const char wc_450[] = "雪で雷を伴う";

#else

PROGMEM const char wc_100[] = "CLEAR";
PROGMEM const char wc_101[] = "PARTLY CLOUDY";
PROGMEM const char wc_102[] = "CLEAR, OCCASIONAL SCATTERED SHOWERS";
PROGMEM const char wc_103[] = "CLEAR, FREQUENT SCATTERED SHOWERS";
PROGMEM const char wc_104[] = "CLEAR, SNOW FLURRIES";
PROGMEM const char wc_105[] = "CLEAR, FREQUENT SNOW FLURRIES";
PROGMEM const char wc_106[] = "CLEAR, OCCASIONAL SCATTERED SHOWERS OR SNOW FLURRIES";
PROGMEM const char wc_107[] = "CLEAR, FREQUENT SCATTERED SHOWERS OR SNOW FLURRIES";
PROGMEM const char wc_108[] = "CLEAR, OCCASIONAL SCATTERED SHOWERS AND/OR THUNDER";
PROGMEM const char wc_110[] = "CLEAR, PARTLY CLOUDY LATER";
PROGMEM const char wc_111[] = "CLEAR, CLOUDY LATER";
PROGMEM const char wc_112[] = "CLEAR, OCCASIONAL SCATTERED SHOWERS LATER";
PROGMEM const char wc_113[] = "CLEAR, FREQUENT SCATTERED SHOWERS LATER";
PROGMEM const char wc_114[] = "CLEAR,RAIN LATER";
PROGMEM const char wc_115[] = "CLEAR, OCCASIONAL SNOW FLURRIES LATER";
PROGMEM const char wc_116[] = "CLEAR, FREQUENT SNOW FLURRIES LATER";
PROGMEM const char wc_117[] = "CLEAR,SNOW LATER";
PROGMEM const char wc_118[] = "CLEAR, RAIN OR SNOW LATER";
PROGMEM const char wc_119[] = "CLEAR, RAIN AND/OR THUNDER LATER";
PROGMEM const char wc_120[] = "OCCASIONAL SCATTERED SHOWERS IN THE MORNING AND EVENING, CLEAR DURING THE DAY";
PROGMEM const char wc_121[] = "OCCASIONAL SCATTERED SHOWERS IN THE MORNING, CLEAR DURING THE DAY";
PROGMEM const char wc_122[] = "CLEAR, OCCASIONAL SCATTERED SHOWERS IN THE EVENING";
PROGMEM const char wc_123[] = "CLEAR IN THE PLAINS, RAIN AND THUNDER NEAR MOUTAINOUS AREAS";
PROGMEM const char wc_124[] = "CLEAR IN THE PLAINS, SNOW NEAR MOUTAINOUS AREAS";
PROGMEM const char wc_125[] = "CLEAR, RAIN AND THUNDER IN THE AFTERNOON";
PROGMEM const char wc_126[] = "CLEAR, RAIN IN THE AFTERNOON";
PROGMEM const char wc_127[] = "CLEAR, RAIN IN THE EVENING";
PROGMEM const char wc_128[] = "CLEAR, RAIN IN THE NIGHT";
PROGMEM const char wc_130[] = "FOG IN THE MORNING, CLEAR LATER";
PROGMEM const char wc_131[] = "FOG AROUND DAWN, CLEAR LATER";
PROGMEM const char wc_132[] = "CLOUDY IN THE MORNING AND EVENING, CLEAR DURING THE DAY";
PROGMEM const char wc_140[] = "CLEAR, FREQUENT SCATTERED SHOWERS AND THUNDER";
PROGMEM const char wc_160[] = "CLEAR, SNOW FLURRIES OR OCCASIONAL SCATTERED SHOWERS";
PROGMEM const char wc_170[] = "CLEAR, FREQUENT SNOW FLURRIES OR SCATTERED SHOWERS";
PROGMEM const char wc_181[] = "CLEAR, SNOW OR RAIN LATER";
PROGMEM const char wc_200[] = "CLOUDY";
PROGMEM const char wc_201[] = "MOSTLY CLOUDY";
PROGMEM const char wc_202[] = "CLOUDY, OCCASIONAL SCATTERED SHOWERS";
PROGMEM const char wc_203[] = "CLOUDY, FREQUENT SCATTERED SHOWERS";
PROGMEM const char wc_204[] = "CLOUDY, OCCASIONAL SNOW FLURRIES";
PROGMEM const char wc_205[] = "CLOUDY FREQUENT SNOW FLURRIES";
PROGMEM const char wc_206[] = "CLOUDY, OCCASIONAL SCATTERED SHOWERS OR SNOW FLURRIES";
PROGMEM const char wc_207[] = "CLOUDY, FREQUENT SCCATERED SHOWERS OR SNOW FLURRIES";
PROGMEM const char wc_208[] = "CLOUDY, OCCASIONAL SCATTERED SHOWERS AND/OR THUNDER";
PROGMEM const char wc_209[] = "FOG";
PROGMEM const char wc_210[] = "CLOUDY, PARTLY CLOUDY LATER";
PROGMEM const char wc_211[] = "CLOUDY, CLEAR LATER";
PROGMEM const char wc_212[] = "CLOUDY, OCCASIONAL SCATTERED SHOWERS LATER";
PROGMEM const char wc_213[] = "CLOUDY, FREQUENT SCATTERED SHOWERS LATER";
PROGMEM const char wc_214[] = "CLOUDY, RAIN LATER";
PROGMEM const char wc_215[] = "CLOUDY, SNOW FLURRIES LATER";
PROGMEM const char wc_216[] = "CLOUDY, FREQUENT SNOW FLURRIES LATER";
PROGMEM const char wc_217[] = "CLOUDY, SNOW LATER";
PROGMEM const char wc_218[] = "CLOUDY, RAIN OR SNOW LATER";
PROGMEM const char wc_219[] = "CLOUDY, RAIN AND/OR THUNDER LATER";
PROGMEM const char wc_220[] = "OCCASIONAL SCCATERED SHOWERS IN THE MORNING AND EVENING, CLOUDY DURING THE DAY";
PROGMEM const char wc_221[] = "CLOUDY OCCASIONAL SCCATERED SHOWERS IN THE MORNING";
PROGMEM const char wc_222[] = "CLOUDY, OCCASIONAL SCCATERED SHOWERS IN THE EVENING";
PROGMEM const char wc_223[] = "CLOUDY IN THE MORNING AND EVENING, PARTLY CLOUDY DURING THE DAY,";
PROGMEM const char wc_224[] = "CLOUDY, RAIN IN THE AFTERNOON";
PROGMEM const char wc_225[] = "CLOUDY, RAIN IN THE EVENING";
PROGMEM const char wc_226[] = "CLOUDY, RAIN IN THE NIGHT";
PROGMEM const char wc_228[] = "CLOUDY, SNOW IN THE AFTERNOON";
PROGMEM const char wc_229[] = "CLOUDY, SNOW IN THE EVENING";
PROGMEM const char wc_230[] = "CLOUDY, SNOW IN THE NIGHT";
PROGMEM const char wc_231[] = "CLOUDY, FOG OR DRIZZLING ON THE SEA AND NEAR SEASHORE";
PROGMEM const char wc_240[] = "CLOUDY, FREQUENT SCCATERED SHOWERS AND THUNDER";
PROGMEM const char wc_250[] = "CLOUDY, FREQUENT SNOW AND THUNDER";
PROGMEM const char wc_260[] = "CLOUDY, SNOW FLURRIES OR OCCASIONAL SCATTERED SHOWERS";
PROGMEM const char wc_270[] = "CLOUDY, FREQUENT SNOW FLURRIES OR SCATTERED SHOWERS";
PROGMEM const char wc_281[] = "CLOUDY, SNOW OR RAIN LATER";
PROGMEM const char wc_300[] = "RAIN";
PROGMEM const char wc_301[] = "RAIN, PARTLY CLOUDY";
PROGMEM const char wc_302[] = "SHOWERS THROUGHOUT THE DAY";
PROGMEM const char wc_303[] = "RAIN,FREQUENT SNOW FLURRIES";
PROGMEM const char wc_304[] = "RAINORSNOW";
PROGMEM const char wc_306[] = "HEAVYRAIN";
PROGMEM const char wc_308[] = "RAINSTORM";
PROGMEM const char wc_309[] = "RAIN,OCCASIONAL SNOW";
PROGMEM const char wc_311[] = "RAIN,CLEAR LATER";
PROGMEM const char wc_313[] = "RAIN,CLOUDY LATER";
PROGMEM const char wc_314[] = "RAIN, FREQUENT SNOW FLURRIES LATER";
PROGMEM const char wc_315[] = "RAIN,SNOW LATER";
PROGMEM const char wc_316[] = "RAIN OR SNOW, CLEAR LATER";
PROGMEM const char wc_317[] = "RAIN OR SNOW, CLOUDY LATER";
PROGMEM const char wc_320[] = "RAIN IN THE MORNING, CLEAR LATER";
PROGMEM const char wc_321[] = "RAIN IN THE MORNING, CLOUDY LATER";
PROGMEM const char wc_322[] = "OCCASIONAL SNOW IN THE MORNING AND EVENING, RAIN DURING THE DAY";
PROGMEM const char wc_323[] = "RAIN, CLEAR IN THE AFTERNOON";
PROGMEM const char wc_324[] = "RAIN, CLEAR IN THE EVENING";
PROGMEM const char wc_325[] = "RAIN, CLEAR IN THE NIGHT";
PROGMEM const char wc_326[] = "RAIN, SNOW IN THE EVENING";
PROGMEM const char wc_327[] = "RAIN,SNOW IN THE NIGHT";
PROGMEM const char wc_328[] = "RAIN, EXPECT OCCASIONAL HEAVY RAINFALL";
PROGMEM const char wc_329[] = "RAIN, OCCASIONAL SLEET";
PROGMEM const char wc_340[] = "SNOWORRAIN";
PROGMEM const char wc_350[] = "RAIN AND THUNDER";
PROGMEM const char wc_361[] = "SNOW OR RAIN, CLEAR LATER";
PROGMEM const char wc_371[] = "SNOW OR RAIN, CLOUDY LATER";
PROGMEM const char wc_400[] = "SNOW";
PROGMEM const char wc_401[] = "SNOW, FREQUENT CLEAR";
PROGMEM const char wc_402[] = "SNOWTHROUGHOUT THE DAY";
PROGMEM const char wc_403[] = "SNOW,FREQUENT SCCATERED SHOWERS";
PROGMEM const char wc_405[] = "HEAVYSNOW";
PROGMEM const char wc_406[] = "SNOWSTORM";
PROGMEM const char wc_407[] = "HEAVYSNOWSTORM";
PROGMEM const char wc_409[] = "SNOW, OCCASIONAL SCCATERED SHOWERS";
PROGMEM const char wc_411[] = "SNOW,CLEAR LATER";
PROGMEM const char wc_413[] = "SNOW,CLOUDY LATER";
PROGMEM const char wc_414[] = "SNOW,RAIN LATER";
PROGMEM const char wc_420[] = "SNOW IN THE MORNING, CLEAR LATER";
PROGMEM const char wc_421[] = "SNOW IN THE MORNING, CLOUDY LATER";
PROGMEM const char wc_422[] = "SNOW, RAIN IN THE AFTERNOON";
PROGMEM const char wc_423[] = "SNOW, RAIN IN THE EVENING";
PROGMEM const char wc_425[] = "SNOW, EXPECT OCCASIONAL HEAVY SNOWFALL";
PROGMEM const char wc_426[] = "SNOW, SLEET LATER";
PROGMEM const char wc_427[] = "SNOW, OCCASIONAL SLEET";
PROGMEM const char wc_450[] = "SNOW AND THUNDER";
#endif

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

#if 0
struct JMA_Prefecures
{
    jma::officecode_t representative; // Representative value of offices.
    std::vector<jma::officecode_t> offices;
};
const JMA_Prefecures prefectures[] = 
{
    {  16000, {  16000, 11000, 13000, 14030, 14100, 15000, 12000, 17000 } },
    {  40000, {  40000, 60000, 70000, 20000, 50000, 30000 } },
    { 130000, { 130000,120000,140000,190000, 90000,100000,110000, 80000,200000 } },
    { 150000, { 150000,170000,180000,160000 } },
    { 230000, { 230000,240000,220000,210000 } },
    { 270000, { 270000,300000,260000,250000,280000,290000 } },
    { 340000, { 340000,310000,330000,320000 } },
    { 370000, { 370000,380000,360000,390000 } },
    { 400000, { 400000,440000,410000,430000,420000,350000 } },
    { 460100, { 460100,450000,460040 } },
    { 471000, { 471000,473000,474000,472000 } },
};
#endif


// Office code string
#if 1
# if 0
PROGMEM const char ocloc_11000[] =  "宗谷地方";
PROGMEM const char ocloc_12000[] =  "上川・留萌地方";
PROGMEM const char ocloc_13000[] = "網走・北見・紋別地方";
PROGMEM const char ocloc_14100[] = "釧路・根室地方";
PROGMEM const char ocloc_14030[] = "十勝地方";
PROGMEM const char ocloc_15000[] = "胆振・日高地方";
PROGMEM const char ocloc_16000[] =  "石狩・空知・後志地方";
PROGMEM const char ocloc_17000[] = "渡島・檜山地方";
PROGMEM const char ocloc_20000[] = "青森県";
PROGMEM const char ocloc_30000[] = "岩手県";
PROGMEM const char ocloc_40000[] = "宮城県";
PROGMEM const char ocloc_50000[] = "秋田県";
PROGMEM const char ocloc_60000[] = "山形県";
PROGMEM const char ocloc_70000[] = "福島県";
PROGMEM const char ocloc_80000[] = "茨城県";
PROGMEM const char ocloc_90000[] = "栃木県";
PROGMEM const char ocloc_100000[] = "群馬県";
PROGMEM const char ocloc_110000[] = "埼玉県";
PROGMEM const char ocloc_120000[] = "千葉県";
PROGMEM const char ocloc_130000[] = "東京都";
PROGMEM const char ocloc_140000[] = "神奈川県";
PROGMEM const char ocloc_150000[] = "新潟県";
PROGMEM const char ocloc_160000[] = "富山県";
PROGMEM const char ocloc_170000[] = "石川県";
PROGMEM const char ocloc_180000[] = "福井県";
PROGMEM const char ocloc_190000[] = "山梨県";
PROGMEM const char ocloc_200000[] = "長野県";
PROGMEM const char ocloc_210000[] = "岐阜県";
PROGMEM const char ocloc_220000[] = "静岡県";
PROGMEM const char ocloc_230000[] = "愛知県";
PROGMEM const char ocloc_240000[] = "三重県";
PROGMEM const char ocloc_250000[] = "滋賀県";
PROGMEM const char ocloc_260000[] = "京都府";
PROGMEM const char ocloc_270000[] = "大阪府";
PROGMEM const char ocloc_280000[] = "兵庫県";
PROGMEM const char ocloc_290000[] = "奈良県";
PROGMEM const char ocloc_300000[] = "和歌山県";
PROGMEM const char ocloc_310000[] = "鳥取県";
PROGMEM const char ocloc_320000[] = "島根県";
PROGMEM const char ocloc_330000[] = "岡山県";
PROGMEM const char ocloc_340000[] = "広島県";
PROGMEM const char ocloc_350000[] = "山口県";
PROGMEM const char ocloc_360000[] = "徳島県";
PROGMEM const char ocloc_370000[] = "香川県";
PROGMEM const char ocloc_380000[] = "愛媛県";
PROGMEM const char ocloc_390000[] = "高知県";
PROGMEM const char ocloc_400000[] = "福岡県";
PROGMEM const char ocloc_410000[] = "佐賀県";
PROGMEM const char ocloc_420000[] = "長崎県";
PROGMEM const char ocloc_430000[] = "熊本県";
PROGMEM const char ocloc_440000[] = "大分県";
PROGMEM const char ocloc_450000[] = "宮崎県";
PROGMEM const char ocloc_460100[] = "鹿児島県";
PROGMEM const char ocloc_460040[] = "奄美地方";
PROGMEM const char ocloc_471000[] = "沖縄本島地方";
PROGMEM const char ocloc_472000[] = "大東島地方";
PROGMEM const char ocloc_473000[] = "宮古島地方";
PROGMEM const char ocloc_474000[] = "八重山地方";
# else
// 
PROGMEM const char ocloc_16000[] =  "札幌";
PROGMEM const char ocloc_40000[] = "仙台";
PROGMEM const char ocloc_130000[] = "東京";
PROGMEM const char ocloc_150000[] = "新潟";
PROGMEM const char ocloc_170000[] = "金沢";
PROGMEM const char ocloc_230000[] = "名古屋";
PROGMEM const char ocloc_270000[] = "大阪";
PROGMEM const char ocloc_340000[] = "広島";
PROGMEM const char ocloc_390000[] = "高知";
PROGMEM const char ocloc_400000[] = "福岡";
PROGMEM const char ocloc_471000[] = "那覇";

# endif
#else

PROGMEM const char ocloc_11000[] = "SOYA"; // Region
PROGMEM const char ocloc_12000[] = "KAMIKAWA RUMOI"; // Region
PROGMEM const char ocloc_13000[] = "ABASHIRI"; // Region
PROGMEM const char ocloc_14100[] = "KUSHIRO NEMURO"; // Region
PROGMEM const char ocloc_14030[] = "TOKACHI"; // Region
PROGMEM const char ocloc_15000[] = "IBURI HIDAKA"; // Region
PROGMEM const char ocloc_16000[] = "ISHIKARI SORACHI"; // Region
PROGMEM const char ocloc_17000[] = "OSHIMA HIYAMA"; // Region
PROGMEM const char ocloc_20000[] = "AOMORI";
PROGMEM const char ocloc_30000[] = "IWATE";
PROGMEM const char ocloc_40000[] = "MIYAGI";
PROGMEM const char ocloc_50000[] = "AKITA";
PROGMEM const char ocloc_60000[] = "YAMAGATA";
PROGMEM const char ocloc_70000[] = "FUKUSHIMA";
PROGMEM const char ocloc_80000[] = "IBARAKI";
PROGMEM const char ocloc_90000[] = "TOCHIGI";
PROGMEM const char ocloc_100000[] = "GUNMA";
PROGMEM const char ocloc_110000[] = "SAITAMA";
PROGMEM const char ocloc_120000[] = "CHIBA";
PROGMEM const char ocloc_130000[] = "TOKYO";
PROGMEM const char ocloc_140000[] = "KANAGAWA";
PROGMEM const char ocloc_150000[] = "NIIGATA";
PROGMEM const char ocloc_160000[] = "TOYAMA";
PROGMEM const char ocloc_170000[] = "ISHIKAWA";
PROGMEM const char ocloc_180000[] = "FUKUI";
PROGMEM const char ocloc_190000[] = "YAMANASHI";
PROGMEM const char ocloc_200000[] = "NAGANO";
PROGMEM const char ocloc_210000[] = "GIFU";
PROGMEM const char ocloc_220000[] = "SHIZUOKA";
PROGMEM const char ocloc_230000[] = "AICHI";
PROGMEM const char ocloc_240000[] = "MIE";
PROGMEM const char ocloc_250000[] = "SHIGA";
PROGMEM const char ocloc_260000[] = "KYOTO";
PROGMEM const char ocloc_270000[] = "OSAKA";
PROGMEM const char ocloc_280000[] = "HYOGO";
PROGMEM const char ocloc_290000[] = "NARA";
PROGMEM const char ocloc_300000[] = "WAKAYAMA";
PROGMEM const char ocloc_310000[] = "TOTTORI";
PROGMEM const char ocloc_320000[] = "SHIANE";
PROGMEM const char ocloc_330000[] = "OKAYAMA";
PROGMEM const char ocloc_340000[] = "HIROSHIMA";
PROGMEM const char ocloc_350000[] = "YAMAGUCHI";
PROGMEM const char ocloc_360000[] = "TOKUSHIMA";
PROGMEM const char ocloc_370000[] = "KAGAWA";
PROGMEM const char ocloc_380000[] = "EHIME";
PROGMEM const char ocloc_390000[] = "KOUCHI";
PROGMEM const char ocloc_400000[] = "FUKUOKA";
PROGMEM const char ocloc_410000[] = "SAGA";
PROGMEM const char ocloc_420000[] = "NAGASAKI";
PROGMEM const char ocloc_430000[] = "KUMAMOTO";
PROGMEM const char ocloc_440000[] = "OOITA";
PROGMEM const char ocloc_450000[] = "MIYAZAKI";
PROGMEM const char ocloc_460100[] = "KAGOSHIMA";
PROGMEM const char ocloc_460040[] = "AMAMI"; // Islands
PROGMEM const char ocloc_471000[] = "OKINAWA"; // Islands
PROGMEM const char ocloc_472000[] = "DAITOU"; // Islands
PROGMEM const char ocloc_473000[] = "MIYAKO"; // Islands
PROGMEM const char ocloc_474000[] = "YAEYAMA"; // Islands
#endif

#if 0
const std::map<jma::officecode_t, const char*> officeCodeTable =
{
    // 北海道
    {  11000, ocloc_11000 },
    {  12000, ocloc_12000 },
    {  13000, ocloc_13000 },
    {  14100, ocloc_14100 },
    {  14030, ocloc_14030 },
    {  15000, ocloc_15000 },
    {  16000, ocloc_16000 },
    {  17000, ocloc_16000 },
    // 東北
    {  20000, ocloc_20000 },
    {  30000, ocloc_30000 },
    {  40000, ocloc_40000 },
    {  50000, ocloc_50000 },
    {  60000, ocloc_60000 },
    {  70000, ocloc_70000 },
    // 関東
    {  80000, ocloc_80000 },
    {  90000, ocloc_90000 },
    { 100000, ocloc_100000 },
    { 110000, ocloc_110000 },
    { 120000, ocloc_120000 },
    { 130000, ocloc_130000 },
    { 140000, ocloc_140000 },
    // 北陸、甲信
    { 150000, ocloc_150000 },
    { 160000, ocloc_160000 },
    { 170000, ocloc_170000 },
    { 180000, ocloc_180000 },
    { 190000, ocloc_190000 },
    { 200000, ocloc_200000 },
    // 東海
    { 210000, ocloc_210000 },
    { 220000, ocloc_220000 },
    { 230000, ocloc_230000 },
    { 240000, ocloc_240000 },
    // 近畿
    { 250000, ocloc_250000 },
    { 260000, ocloc_260000 },
    { 270000, ocloc_270000 },
    { 280000, ocloc_280000 },
    { 290000, ocloc_290000 },
    { 300000, ocloc_300000 },
    // 中国
    { 310000, ocloc_310000 },
    { 320000, ocloc_320000 },
    { 330000, ocloc_330000 },
    { 340000, ocloc_340000 },
    { 350000, ocloc_350000 },
    // 四国
    { 360000, ocloc_360000 },
    { 370000, ocloc_370000 },
    { 380000, ocloc_380000 },
    { 390000, ocloc_390000 },
    // 九州
    { 400000, ocloc_400000 },
    { 410000, ocloc_410000 },
    { 420000, ocloc_420000 },
    { 430000, ocloc_430000 },
    { 440000, ocloc_440000 },
    { 450000, ocloc_450000 },
    { 460100, ocloc_460100 },
    { 460040, ocloc_460040 },
    // 沖縄
    { 471000, ocloc_471000 },
    { 472000, ocloc_472000 },
    { 473000, ocloc_473000 },
    { 474000, ocloc_474000 },
};

#else

const std::map<jma::officecode_t, const char*> officeCodeTable =
{
    {  16000, ocloc_16000 },
    {  40000, ocloc_40000 },
    { 130000, ocloc_130000 },
    { 150000, ocloc_150000 },
    { 170000, ocloc_170000 },
    { 230000, ocloc_230000 },
    { 270000, ocloc_270000 },
    { 340000, ocloc_340000 },
    { 390000, ocloc_390000 },
    { 400000, ocloc_400000 },
    { 471000, ocloc_471000 },
};

#endif

// Weather code to weather icon
const std::map<jma::weathercode_t, int16_t> iconTable =
{
    { 100, 100 },
    { 101, 101 },
    { 102, 102 },
    { 103, 102 },
    { 104, 104 },
    { 105, 104 },
    { 106, 102 },
    { 107, 102 },
    { 108, 102 },
    { 110, 110 },
    { 111, 110 },
    { 112, 112 },
    { 113, 112 },
    { 114, 112 },
    { 115, 115 },
    { 116, 115 },
    { 117, 115 },
    { 118, 112 },
    { 119, 112 },
    { 120, 102 },
    { 121, 102 },
    { 122, 112 },
    { 123, 100 },
    { 124, 100 },
    { 125, 112 },
    { 126, 112 },
    { 127, 112 },
    { 128, 112 },
    { 130, 100 },
    { 131, 100 },
    { 132, 101 },
    { 140, 102 },
    { 160, 104 },
    { 170, 104 },
    { 181, 115 },
    { 200, 200 },
    { 201, 201 },
    { 202, 202 },
    { 203, 202 },
    { 204, 204 },
    { 205, 204 },
    { 206, 202 },
    { 207, 202 },
    { 208, 202 },
    { 209, 200 },
    { 210, 210 },
    { 211, 210 },
    { 212, 212 },
    { 213, 212 },
    { 214, 212 },
    { 215, 215 },
    { 216, 215 },
    { 217, 215 },
    { 218, 212 },
    { 219, 212 },
    { 220, 202 },
    { 221, 202 },
    { 222, 212 },
    { 223, 201 },
    { 224, 212 },
    { 225, 212 },
    { 226, 212 },
    { 228, 215 },
    { 229, 215 },
    { 230, 215 },
    { 231, 200 },
    { 240, 202 },
    { 250, 204 },
    { 260, 204 },
    { 270, 204 },
    { 281, 215 },
    { 300, 300 },
    { 301, 301 },
    { 302, 302 },
    { 303, 303 },
    { 304, 300 },
    { 306, 300 },
    { 308, 308 },
    { 309, 303 },
    { 311, 311 },
    { 313, 313 },
    { 314, 314 },
    { 315, 314 },
    { 316, 311 },
    { 317, 313 },
    { 320, 311 },
    { 321, 313 },
    { 322, 303 },
    { 323, 311 },
    { 324, 311 },
    { 325, 311 },
    { 326, 314 },
    { 327, 314 },
    { 328, 300 },
    { 329, 300 },
    { 340, 400 },
    { 350, 300 },
    { 361, 411 },
    { 371, 413 },
    { 400, 400 },
    { 401, 401 },
    { 402, 402 },
    { 403, 403 },
    { 405, 400 },
    { 406, 406 },
    { 407, 406 },
    { 409, 403 },
    { 411, 411 },
    { 413, 413 },
    { 414, 414 },
    { 420, 411 },
    { 421, 413 },
    { 422, 414 },
    { 423, 414 },
    { 425, 400 },
    { 426, 400 },
    { 427, 400 },
    { 450, 400 },
};

//
}

namespace jma
{
// DigiCert Global Root CA of www.jma.go.jp.
// Exxpire at Mon, 10 Nov 2031 00:00:00 GMT.
PROGMEM const char root_ca_JMA[] =
        "-----BEGIN CERTIFICATE-----" "\n"
        "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh" "\n" 
        "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3" "\n" 
        "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD" "\n" 
        "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT" "\n" 
        "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j" "\n" 
        "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG" "\n" 
        "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB" "\n" 
        "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97" "\n" 
        "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt" "\n" 
        "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P" "\n" 
        "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4" "\n" 
        "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO" "\n" 
        "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR" "\n" 
        "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw" "\n"
        "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr" "\n" 
        "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg" "\n" 
        "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF" "\n" 
        "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls" "\n" 
        "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk" "\n" 
        "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4="  "\n" 
        "-----END CERTIFICATE-----" "\n"
        ;

const char* weatherCodeToString(const weathercode_t wc)
{
    return weatherCodeTable.count(wc) == 1 ? weatherCodeTable.at(wc) : "";
}

const char* officesCodeToString(const officecode_t oc)
{
    return officeCodeTable.count(oc) == 1 ? officeCodeTable.at(oc) : "";
}

int16_t weatherCodeToIcon(const weathercode_t wc)
{
    return iconTable.count(wc) == 1 ? iconTable.at(wc) : 0;
}

//
}
