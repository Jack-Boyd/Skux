#pragma once

#include <JuceHeader.h>
#include "Filter.h"
#include "Distortion.h"

class SkuxAudioProcessor  : public juce::AudioProcessor
{
public:
  SkuxAudioProcessor();
  ~SkuxAudioProcessor() override;

  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

 #ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
 #endif

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  using APVTS = juce::AudioProcessorValueTreeState;
  static APVTS::ParameterLayout createParameterLayout();
  
  APVTS apvts{*this, nullptr, "Parameters", createParameterLayout()};

private:
  Filter m_filterProcessor;
  Distortion m_distortionProcessor;
  
  juce::AudioParameterFloat *m_distDriveParam{nullptr};
  juce::AudioParameterFloat *m_distMixParam{nullptr};
  juce::AudioParameterChoice* m_distTypeParam{nullptr};
  
  juce::AudioParameterFloat* m_distFilterCutoffParam{nullptr};
  juce::AudioParameterChoice* m_distFilterRoutingParam{nullptr};
  juce::AudioParameterFloat* m_distFilterQParam{nullptr};
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SkuxAudioProcessor)
};
