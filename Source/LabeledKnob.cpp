#include "LabeledKnob.h"

LabeledKnob::LabeledKnob(const juce::String& name)
{
  slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
  slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white.withAlpha(0.7f));
  slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
  addAndMakeVisible(slider);

  label.setText(name, juce::dontSendNotification);
  label.setJustificationType(juce::Justification::centred);
  label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.6f));
  label.setFont(juce::FontOptions(11.f, juce::Font::bold));
  addAndMakeVisible(label);
}

void LabeledKnob::resized()
{
  auto b = getLocalBounds();
  label.setBounds(b.removeFromTop(16));
  slider.setBounds(b);
}
