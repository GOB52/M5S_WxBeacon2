/*!
  AquesTalk play with M5.Speaker
  From https://github.com/m5stack/M5Unified/tree/master/examples/Advanced/Speak_with_AquesTalk
 */
#ifndef AQ_TALK_HPP
#define AQ_TALK_HPP

#include <cstdint>

namespace aq_talk
{

bool setup();
void finalize();

bool busy();
uint32_t getGain();

void waitAquesTalk(void);
void stopAquesTalk(void);
void playAquesTalk(const char *koe, uint16_t speed = 100, uint16_t lenPause = 256U);

//
}
#endif
