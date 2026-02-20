#pragma once
#include <JuceHeader.h>

class Filter
{
public:
  void prepare(const juce::dsp::ProcessSpec& spec);
  void process(juce::AudioBuffer<float>& buffer, float cutoff, float q,
               float mix);
  void reset();

private:
  using MonoFilter = juce::dsp::IIR::Filter<float>;
  using FilterCoeffs = juce::dsp::IIR::Coefficients<float>;
  using StereoFilter = juce::dsp::ProcessorDuplicator<MonoFilter, FilterCoeffs>;
  using FilterChain = juce::dsp::ProcessorChain<StereoFilter, StereoFilter>;

  FilterChain m_filter;
  juce::AudioBuffer<float> m_filterMixBuffer;
  double m_sampleRate = 44100.0;
  float m_lastFilterCutoff = -1.f;
  float m_lastFilterQ = -1.f;
};
