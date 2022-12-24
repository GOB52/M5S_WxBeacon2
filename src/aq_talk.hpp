/*!
  AquesTalk play with M5.Speaker
  From https://github.com/m5stack/M5Unified/tree/master/examples/Advanced/Speak_with_AquesTalk
 */
#ifndef AQ_TALK_HPP
#define AQ_TALK_HPP

#include "jma/jma_definition.hpp"
//#include <freertos/FreeRTOS.h>

namespace aq_talk
{
using aq_talk_callback = void(*)();
bool initialize(const UBaseType_t priority, const BaseType_t core, aq_talk_callback f = nullptr);
void finalize();

bool busy();
uint32_t getGain();

void waitAquesTalk(void);
void stopAquesTalk(void);
bool playAquesTalk(const char *koe, const uint16_t speed = 100, const uint16_t lenPause = 0xFFFFU);

const char* weatherCodeToTalk(const jma::weathercode_t wc);
const char* officeCodeToTalk(const jma::officecode_t oc);
//
}
#endif
