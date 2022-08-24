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

namespace
{
constexpr uint8_t m5spk_virtual_channel = 0;
constexpr std::size_t LEN_FRAME = 32;

uint32_t workbuf[AQ_SIZE_WORKBUF];
TaskHandle_t task_handle = nullptr;
volatile bool is_talking = false;
volatile uint32_t level = 0;

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
            if (CAqTkPicoF_SyntheFrame(wav[tri_index], &len)) { is_talking = false; break; }
            if(len)
            {
                calcGainAverage(wav[tri_index], len);
                M5.Speaker.playRaw(wav[tri_index], len, 8000, false, 1, m5spk_virtual_channel, false);
                tri_index = tri_index < 2 ? tri_index + 1 : 0;
            }
        }
    }
}
//
}

namespace aq_talk
{
bool setup()
{
    if(!task_handle)
    {
        xTaskCreateUniversal(talk_task, "aq_talk_task", 4096, nullptr, 1, &task_handle, APP_CPU_NUM);
        return task_handle
                && CAqTkPicoF_Init(workbuf, LEN_FRAME, "XXX-XXX-XXX") == 0;
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
}

void stopAquesTalk(void)
{
    if (is_talking) { is_talking = false; vTaskDelay(1); }
}

void playAquesTalk(const char *koe, uint16_t speed, uint16_t lenPause)
{
    stopAquesTalk();
    auto ret = CAqTkPicoF_SetKoe(reinterpret_cast<const uint8_t*>(koe), speed, lenPause);
    if(ret == 0)
    {
        is_talking = true;
        xTaskNotifyGive(task_handle);
    }
}
//
}
