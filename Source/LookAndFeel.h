#pragma once
#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
  LookAndFeel();

  void drawRotarySlider(juce::Graphics& g, int x, int y, int width,
                        int height, float sliderPos, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider& slider) override;

  void drawLabel(juce::Graphics& g, juce::Label& label) override;
};
