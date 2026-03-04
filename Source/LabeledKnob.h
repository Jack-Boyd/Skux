#pragma once

#include <JuceHeader.h>

class LabeledKnob : public juce::Component
{
public:
  LabeledKnob(const juce::String& name);

  void resized() override;

  juce::Slider slider;
  juce::Label label;
};
