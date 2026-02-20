#pragma once
#include <JuceHeader.h>

class Distortion
{
public:
  void process(juce::AudioBuffer<float>& buffer, float drive,
               float mix, int clipType);

private:
  static inline float fastTanh(float value)
  {
    value = std::clamp(value, -5.f, 5.f);
    const float v2 = value * value;
    return value * (945.f + v2 * (105.f + v2))
           / (945.f + v2 * (420.f + v2 * 15.f));
  }
};
