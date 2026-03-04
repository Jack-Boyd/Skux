#include "LabeledComboBox.h"

LabeledComboBox::LabeledComboBox(const juce::String& name)
{
  addAndMakeVisible(comboBox);

  label.setText(name, juce::dontSendNotification);
  label.setJustificationType(juce::Justification::centred);
  label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.6f));
  label.setFont(juce::FontOptions(11.f, juce::Font::bold));
  addAndMakeVisible(label);
}

void LabeledComboBox::resized()
{
  auto b = getLocalBounds();
  label.setBounds(b.removeFromTop(16));
  auto comboArea = b.reduced(4, 0);
  comboBox.setBounds(comboArea.withSizeKeepingCentre(comboArea.getWidth(),
                                                     juce::jmin(24, comboArea.getHeight())));
}
