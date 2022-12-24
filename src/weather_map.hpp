/*
  Weather map
 */
#ifndef WEATHER_MAP_HPP
#define WEATHER_MAP_HPP

#include <M5GFX.h>
#include <vector>
#include "jma/jma_definition.hpp"
#include<gob_datetime.hpp>

class WeatherMap
{
  public:
    struct IconInfo
    {
        IconInfo(const jma::officecode_t ocv, const jma::weathercode_t wcv) : oc(ocv), wc(wcv) {}
        jma::officecode_t oc{};
        jma::weathercode_t wc{};
    };

    WeatherMap() { _icons.reserve(16); }

    inline void show() { show(true); }
    inline void hide() { show(false); }

    void setDatetime(const goblib::datetime::OffsetDateTime& odt) { _odt =odt; }
    inline void clearIcon() { _icons.clear(); }
    void addIcon(const jma::officecode_t oc, const jma::weathercode_t wc) { _icons.emplace_back(oc, wc); }

    void render(m5gfx::M5GFX* dst, bool forece = false);

  private:
    void show(bool b) { _show = b; }
    inline bool isShow() const { return _show; }
    
  private:
    bool _show{false};
    goblib::datetime::OffsetDateTime _odt{};
    std::vector<IconInfo> _icons{};
};
#endif
