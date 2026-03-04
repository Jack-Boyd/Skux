#pragma once

#include <JuceHeader.h>
#include "Oscilloscope.h"
#include "PluginProcessor.h"

class SkuxAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  SkuxAudioProcessorEditor(SkuxAudioProcessor&);
  ~SkuxAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  SkuxAudioProcessor& audioProcessor;
  Oscilloscope oscilloscope;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkuxAudioProcessorEditor)
};
