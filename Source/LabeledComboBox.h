#pragma once
#include <JuceHeader.h>

class LabeledComboBox : public juce::Component
{
public:
  LabeledComboBox(const juce::String& name);

  void resized() override;

  juce::ComboBox comboBox;
  juce::Label label;
};

