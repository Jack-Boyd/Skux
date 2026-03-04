#pragma once

#include <JuceHeader.h>
#include "Filter.h"
#include "Distortion.h"
#include <array>

template <size_t BlockSize, int NumBlocks>
class ScopeDataQueue
{
public:
  ScopeDataQueue() = default;

  void push(const float* data, size_t numSamples)
  {
    for (size_t i = 0; i < numSamples; ++i) {
      currentBlock[sampleIndex++] = data[i];
      if (sampleIndex == BlockSize) {
        int start1, size1, start2, size2;
        fifo.prepareToWrite(1, start1, size1, start2, size2);

        if (size1 > 0) {
          std::copy(
            currentBlock.begin(),
            currentBlock.end(),
            blocks[static_cast<size_t>(start1)].begin());
        }

        fifo.finishedWrite(size1 + size2);
        sampleIndex = 0;
      }
    }
  }

  bool pull(std::array<float, BlockSize>& outBlock)
  {
    bool hasNewData = false;

    int numReady = fifo.getNumReady();

    while (numReady > 0) {
      int start1, size1, start2, size2;
      fifo.prepareToRead(1, start1, size1, start2, size2);

      if (size1 > 0) {
        std::copy(
          blocks[static_cast<size_t>(start1)].begin(),
          blocks[static_cast<size_t>(start1)].end(),
          outBlock.begin());
        hasNewData = true;
      }

      fifo.finishedRead(size1 + size2);
      numReady = fifo.getNumReady();
    }

    return hasNewData;
  }

private:
  juce::AbstractFifo fifo{ NumBlocks };
  std::array<std::array<float, BlockSize>, NumBlocks> blocks;
  std::array<float, BlockSize> currentBlock;
  size_t sampleIndex = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeDataQueue)
};

inline constexpr size_t ScopeBlockSize = 512;
inline constexpr int ScopeNumBlocks = 30;

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
  
  ScopeDataQueue<ScopeBlockSize, ScopeNumBlocks>& getScopeQueue() {
    return m_scopeQueue;
  }
private:
  Filter m_filterProcessor;
  Distortion m_distortionProcessor;
  
  juce::AudioParameterFloat *m_distDriveParam{nullptr};
  juce::AudioParameterFloat *m_distMixParam{nullptr};
  juce::AudioParameterChoice* m_distTypeParam{nullptr};
  
  juce::AudioParameterFloat* m_distFilterCutoffParam{nullptr};
  juce::AudioParameterChoice* m_distFilterRoutingParam{nullptr};
  juce::AudioParameterFloat* m_distFilterQParam{nullptr};
  
  ScopeDataQueue<ScopeBlockSize, ScopeNumBlocks> m_scopeQueue;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SkuxAudioProcessor)
};
