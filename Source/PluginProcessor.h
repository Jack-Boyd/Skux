/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SkuxAudioProcessor  : public juce::AudioProcessor
{
public:
  //==============================================================================
  SkuxAudioProcessor();
  ~SkuxAudioProcessor() override;

  //==============================================================================
  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

 #ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
 #endif

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  using APVTS = juce::AudioProcessorValueTreeState;
  static APVTS::ParameterLayout createParameterLayout();
  
  APVTS apvts{*this, nullptr, "Parameters", createParameterLayout()};

private:
  juce::AudioParameterChoice* typeParam = nullptr;
  juce::AudioParameterFloat *mixParam{nullptr};
  juce::AudioParameterFloat *driveParam{nullptr};
  
  static inline float fastTanh(float value)
  {
    value = std::clamp(value, -5.f, 5.f);
    const float v2 = value * value;
    return value * (945.f + v2 * (105.f + v2)) / (945.f + v2 * (420.f + v2 * 15.f));
  }
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SkuxAudioProcessor)
};
