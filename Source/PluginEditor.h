/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OscilloscopeComponent : public juce::Component, private juce::Timer
{
public:
  OscilloscopeComponent(ScopeDataQueue<ScopeBlockSize, ScopeNumBlocks>& queue) : m_queue(queue)
  {
    m_displayBuffer.fill(0.0f);
    startTimerHz(30);
  }

  void paint(juce::Graphics& g) override
  {
    auto bounds = getLocalBounds().toFloat();

    g.fillAll(juce::Colour(0xff1a1a2e));

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRect(bounds, 1.f);

    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawHorizontalLine(
      static_cast<int>(bounds.getCentreY()),
      bounds.getX(), bounds.getRight());

    if (!m_hasData)
      return;

    juce::Path waveform;
    const auto numSamples = static_cast<int>(m_displayBuffer.size());
    const float xScale = bounds.getWidth() / static_cast<float>(numSamples - 1);
    const float yCentre = bounds.getCentreY();
    const float yScale = bounds.getHeight() * 0.45f;

    waveform.startNewSubPath(
      bounds.getX(),
      yCentre - m_displayBuffer[0] * yScale);

    for (int i = 1; i < numSamples; ++i) {
      waveform.lineTo(
        bounds.getX() + static_cast<float>(i) * xScale,
        yCentre - m_displayBuffer[static_cast<size_t>(i)] * yScale);
    }

    g.setColour(juce::Colour(0xff00e5ff));
    g.strokePath(
      waveform,
      juce::PathStrokeType(
        1.5f, juce::PathStrokeType::curved));
  }

private:
  void timerCallback() override
  {
    if (m_queue.pull(m_displayBuffer)) {
      m_hasData = true;
      repaint();
    }
  }

  ScopeDataQueue<ScopeBlockSize, ScopeNumBlocks>& m_queue;
  std::array<float, ScopeBlockSize> m_displayBuffer;
  bool m_hasData = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopeComponent)
};  

class SkuxAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  SkuxAudioProcessorEditor(SkuxAudioProcessor&);
  ~SkuxAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  SkuxAudioProcessor& audioProcessor;
  OscilloscopeComponent oscilloscope;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkuxAudioProcessorEditor)
};
