#pragma once

#include <JuceHeader.h>
#include "LabeledComboBox.h"
#include "LabeledKnob.h"
#include "LookAndFeel.h"
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
  LookAndFeel skuxLookAndFeel;

  Oscilloscope oscilloscope;

  LabeledKnob driveKnob{"DRIVE"};
  LabeledKnob mixKnob{"MIX"};
  LabeledComboBox distTypeBox{"TYPE"};

  LabeledKnob cutoffKnob{"CUTOFF"};
  LabeledKnob qKnob{"Q"};
  LabeledComboBox filterRoutingBox{"ROUTING"};

  using APVTS = juce::AudioProcessorValueTreeState;
  using SliderAttachment = APVTS::SliderAttachment;
  using ComboBoxAttachment = APVTS::ComboBoxAttachment;

  std::unique_ptr<SliderAttachment> driveAttachment;
  std::unique_ptr<SliderAttachment> mixAttachment;
  std::unique_ptr<ComboBoxAttachment> distTypeAttachment;
  std::unique_ptr<SliderAttachment> cutoffAttachment;
  std::unique_ptr<SliderAttachment> qAttachment;
  std::unique_ptr<ComboBoxAttachment> filterRoutingAttachment;

  juce::Label distortionSectionLabel;
  juce::Label filterSectionLabel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkuxAudioProcessorEditor)
};
