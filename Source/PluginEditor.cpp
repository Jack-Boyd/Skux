#include "PluginProcessor.h"
#include "PluginEditor.h"

SkuxAudioProcessorEditor::SkuxAudioProcessorEditor(SkuxAudioProcessor& p)
  : AudioProcessorEditor(&p), audioProcessor(p), oscilloscope(p.getScopeQueue())
{
  addAndMakeVisible(oscilloscope);
  setSize(600, 300);
}

SkuxAudioProcessorEditor::~SkuxAudioProcessorEditor() {}

void SkuxAudioProcessorEditor::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colour(0xff0f0f23));
}

void SkuxAudioProcessorEditor::resized()
{
  oscilloscope.setBounds(getLocalBounds().reduced(10));
}
